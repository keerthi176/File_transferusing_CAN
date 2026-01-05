#include "can_thread.h"
#include "can.h"
#include "common_utils.h"
#include "firmware_image.h"

#define ID_HELLO_WORLD      (0x100)

static can_cfg_t g_can0_cfg_local;
static can_cfg_t g_can1_cfg_local;
extern bool CAN_Flag;
extern TX_BYTE_POOL g_can_message_pool;

/* Firmware Receiver Defines and State */
#define RECONSTRUCTION_BUFFER_SIZE (186360)

typedef enum e_receiver_state
{
    STATE_IDLE,
    STATE_RECEIVING,
    STATE_FINISHED,
    STATE_ERROR
} receiver_state_t;

static receiver_state_t g_receiver_state = STATE_IDLE;
static uint8_t g_reconstruction_buffer[RECONSTRUCTION_BUFFER_SIZE];
static uint32_t g_received_size = 0;
static uint32_t g_expected_file_size = 0;
static uint16_t g_next_sequence_no = 0;


static can_node_t g_can_nodes[2] =
{
    [0] = {
            .p_ctrl = &g_can0_ctrl,
            .p_cfg = &g_can0_cfg_local,
            .tx_complete = false,
            .rx_pending = false,
            .error = false
          },
    [1] = {
            .p_ctrl = &g_can1_ctrl,
            .p_cfg = &g_can1_cfg_local,
            .tx_complete = false,
            .rx_pending = false,
            .error = false
          }
};

void can_callback(can_callback_args_t *p_args);
void can_init(void);
void can_run(void);
void process_message(can_node_t *p_node);
fsp_err_t can_transmit(can_node_t *p_node, can_frame_t *p_frame);
static void can_handle_error(fsp_err_t err,  const char * err_str);

#define CAN_QUEUE_MAX_MESSAGES  (64)

TX_QUEUE g_can_tx_queue;

static ULONG g_can_tx_queue_buffer[CAN_QUEUE_MAX_MESSAGES];


void can_thread_entry(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    R_FSP_VersionGet(&version);

    /* CREATE THE MESSAGE QUEUE */
    UINT tx_status = tx_queue_create(&g_can_tx_queue,
                                "CAN TX Queue",
                                1,      // Message size is 1 ULONG (pointers are 1 ULONG)
                                &g_can_tx_queue_buffer,
                                sizeof(g_can_tx_queue_buffer));
    if (TX_SUCCESS != tx_status)
    {
        APP_ERR_PRINT("Failed to create CAN TX message queue.\r\n");
        APP_ERR_TRAP(tx_status);
    }

    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

#define CAN_TX_TIMEOUT_MS (100)

    //initialise the can bus.
    can_init();

    while (1)
    {
		UINT tx_status;
		can_msg_t *p_can_msg = NULL;

		tx_status = tx_queue_receive(&g_can_tx_queue, (VOID *)&p_can_msg, TX_NO_WAIT);

		if (TX_SUCCESS == tx_status)
		{
			can_frame_t frame;
			memset(&frame, 0, sizeof(can_frame_t));
			frame.id_mode = CAN_ID_MODE_STANDARD;
			frame.type = CAN_FRAME_TYPE_DATA;

			fsp_err_t fsp_status = FSP_SUCCESS;

			switch (p_can_msg->msg_type)
			{
				case CAN_MSG_TYPE_SOT:
				{
					APP_PRINT("Sending SOT. File size: %lu\r\n", p_can_msg->payload.file_size);
					frame.id = CAN_ID_FILE_CONTROL;
					frame.data_length_code = 5;
					frame.data[0] = SOT_PACKET;
					memcpy(&frame.data[1], &p_can_msg->payload.file_size, sizeof(p_can_msg->payload.file_size));
					break;
				}

				case CAN_MSG_TYPE_DATA:
				{
					frame.id = CAN_ID_FILE_DATA;
					frame.data_length_code = (uint8_t) (2 + p_can_msg->payload.data_packet.length);
					memcpy(&frame.data[0], &p_can_msg->payload.data_packet.sequence_no, sizeof(uint16_t));
					memcpy(&frame.data[2], p_can_msg->payload.data_packet.data, p_can_msg->payload.data_packet.length);
					break;
				}

				case CAN_MSG_TYPE_EOT:
				{
					APP_PRINT("Sending EOT.\r\n");
					frame.id = CAN_ID_FILE_CONTROL;
					frame.data_length_code = 1;
					frame.data[0] = EOT_PACKET;
					break;
				}

				default:
					APP_PRINT("Received unknown message type.\r\n");
					tx_byte_release(p_can_msg);
					continue;
			}

			g_can_nodes[0].tx_complete = false;
			fsp_status = R_CAN_Write(g_can_nodes[0].p_ctrl, 0, &frame);
			if (FSP_SUCCESS != fsp_status)
			{
				APP_ERR_PRINT("R_CAN_Write failed with status: %d\r\n", fsp_status);
			}
			else
			{
				UINT wait_count = 0;
				while(false == g_can_nodes[0].tx_complete)
				{
					tx_thread_sleep(1);
					wait_count++;
					if (wait_count > CAN_TX_TIMEOUT_MS)
					{
						 APP_ERR_PRINT("CAN TX timeout!\r\n");
						 break;
					}
				}
			}

			tx_byte_release(p_can_msg);
		}

		if (g_can_nodes[1].rx_pending)
		{
			g_can_nodes[1].rx_pending = false;
			process_message(&g_can_nodes[1]);
		}
		if (g_can_nodes[1].error)
		{
			g_can_nodes[1].error = false;
			APP_ERR_PRINT("\r\nCAN Bus Error on Channel %d!\r\n", g_can_nodes[1].p_cfg->channel);
		}

		if (TX_QUEUE_EMPTY == tx_status)
		{
				tx_thread_sleep(5);
			        }
			    }
			}
			
/*******************************************************************************************************************//**
 * @brief       Sends the firmware image over CAN using the defined protocol.
 * @param[IN]   None
 * @retval      UINT                    TX_SUCCESS if successful, otherwise an error code.
 **********************************************************************************************************************/
UINT send_firmware_over_can(void)
{
    UINT status = TX_SUCCESS;
    can_msg_t *p_can_msg = NULL;
    uint32_t current_offset = 0;
    uint16_t sequence_no = 0;
    uint32_t data_packets_queued = 0;

    APP_PRINT("Queueing SOT message...\r\n");
    status = tx_byte_allocate(&g_can_message_pool, (VOID **) &p_can_msg, sizeof(can_msg_t), TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        APP_ERR_PRINT("Failed to allocate memory for SOT message\r\n");
    }
    else
    {
        p_can_msg->msg_type = CAN_MSG_TYPE_SOT;
        p_can_msg->payload.file_size = firmware_len;

        status = tx_queue_send(&g_can_tx_queue, &p_can_msg, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            APP_ERR_PRINT("Failed to send SOT message to queue\r\n");
            tx_byte_release(p_can_msg);
        }
    }

    if (TX_SUCCESS != status)
    {
        return status;
    }

    APP_PRINT("Queueing DATA messages (firmware size: %lu bytes)...\r\n", firmware_len);
    while (current_offset < firmware_len)
    {
        status = tx_byte_allocate(&g_can_message_pool, (VOID **) &p_can_msg, sizeof(can_msg_t), TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            APP_ERR_PRINT("Failed to allocate memory for DATA message\r\n");
            break;
        }

        uint32_t bytes_to_send = (firmware_len - current_offset > CAN_DATA_MAX_SIZE) ?
                                  CAN_DATA_MAX_SIZE : (firmware_len - current_offset);

        p_can_msg->msg_type = CAN_MSG_TYPE_DATA;
        p_can_msg->payload.data_packet.length = (uint8_t)bytes_to_send;
        p_can_msg->payload.data_packet.sequence_no = sequence_no;
        memcpy(p_can_msg->payload.data_packet.data, &firmware_data[current_offset], bytes_to_send);

        status = tx_queue_send(&g_can_tx_queue, &p_can_msg, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            APP_ERR_PRINT("Failed to send DATA message to queue (seq: %u)\r\n", sequence_no);
            tx_byte_release(p_can_msg);
            break;
        }

        current_offset += bytes_to_send;
        sequence_no++;
        data_packets_queued++;
    }
    APP_PRINT("Queued %lu DATA packets.\r\n", data_packets_queued);

    if (TX_SUCCESS == status)
    {
        APP_PRINT("Queueing EOT message...\r\n");
        status = tx_byte_allocate(&g_can_message_pool, (VOID **) &p_can_msg, sizeof(can_msg_t), TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            APP_ERR_PRINT("Failed to allocate memory for EOT message\r\n");
        }
        else
        {
            p_can_msg->msg_type = CAN_MSG_TYPE_EOT;

            status = tx_queue_send(&g_can_tx_queue, &p_can_msg, TX_WAIT_FOREVER);
            if (TX_SUCCESS != status)
            {
                APP_ERR_PRINT("Failed to send EOT message to queue\r\n");
                tx_byte_release(p_can_msg);
            }
        }
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function responsible for initializing the CAN communication.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
void can_init(void)
{
    fsp_err_t status = FSP_SUCCESS;

    g_can0_cfg_local = g_can0_cfg;
    g_can0_cfg_local.p_callback = can_callback;
    g_can0_cfg_local.p_context = &g_can_nodes[0];

    status = R_CAN_Open(g_can_nodes[0].p_ctrl, g_can_nodes[0].p_cfg);
    can_handle_error(status, "\r\n **CAN0 Open API failed**");

    /* Disable loopback mode to allow physical transmission between CAN0 and CAN1 */
    // status = R_CAN_ModeTransition(g_can_nodes[0].p_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_LOOPBACK_EXTERNAL);
    // can_handle_error(status, "\r\n **CAN0 External Loopback transition failed**");
    // APP_PRINT("\r\nCAN0 configured for External Loopback Mode to enable self-reception.\r\n");

    g_can1_cfg_local = g_can1_cfg;
    g_can1_cfg_local.p_callback = can_callback;
    g_can1_cfg_local.p_context = &g_can_nodes[1];

    status = R_CAN_Open(g_can_nodes[1].p_ctrl, g_can_nodes[1].p_cfg);
    can_handle_error(status, "\r\n **CAN1 Open API failed**");
}

/*******************************************************************************************************************//**
 * @brief     This function responsible for handling the errors.
 * @param[IN] fsp_err_t status,  char err_str
 * @retval    None
 **********************************************************************************************************************/
static void can_handle_error(fsp_err_t status,  const char * err_str)
{
    if(FSP_SUCCESS != status)
    {
        APP_ERR_PRINT(err_str);

        if(RESET_VALUE != g_can0_ctrl.open)
        {
            if(FSP_SUCCESS != R_CAN_Close (&g_can0_ctrl))
            {
                APP_ERR_PRINT("\r\nCAN0 Close API failed\r\n");
            }
        }

        if(RESET_VALUE != g_can1_ctrl.open)
        {
            if(FSP_SUCCESS != R_CAN_Close (&g_can1_ctrl))
            {
                APP_ERR_PRINT("\r\nCAN1 Close API failed\r\n");
            }
        }

        APP_ERR_TRAP(status);
    }
}

/***********************************************************************************************************************
 * @brief     This function responsible to trigger when the interrupt hits.
 * @param[IN] can_callback_args_t *p_args
 * @retval    None
 **********************************************************************************************************************/
void can_callback(can_callback_args_t *p_args)
{
    can_node_t *p_node = (can_node_t *)p_args->p_context;

    switch(p_args->event)
    {
        case CAN_EVENT_TX_COMPLETE:
        {
            p_node->tx_complete = true;
            break;
        }

        case CAN_EVENT_RX_COMPLETE:
        {
            p_node->frame = p_args->frame;
            p_node->rx_pending = true;
            break;
        }

        case CAN_EVENT_ERR_WARNING:
        case CAN_EVENT_ERR_PASSIVE:
        case CAN_EVENT_ERR_BUS_OFF:
        case CAN_EVENT_FIFO_MESSAGE_LOST:
        {
            p_node->error = true;
            break;
        }

        default:
        {
            break;
        }
    }
}

/***********************************************************************************************************************
 * @brief     This function responsible for processing the received message.
 * @param[IN] can_node_t *p_node
 * @retval    None
 **********************************************************************************************************************/
void process_message(can_node_t *p_node)
{
    can_frame_t * p_frame = &p_node->frame;

    switch (g_receiver_state)
    {
        case STATE_IDLE:
        {
            if ((p_frame->id == CAN_ID_FILE_CONTROL) && (p_frame->data[0] == SOT_PACKET))
            {
                if (p_frame->data_length_code == 5)
                {
                    memcpy(&g_expected_file_size, &p_frame->data[1], sizeof(g_expected_file_size));
                    if (g_expected_file_size > RECONSTRUCTION_BUFFER_SIZE)
                    {
                        APP_ERR_PRINT("\r\n[RX] SOT Error: Firmware size (%lu) exceeds buffer size (%d).\r\n", g_expected_file_size, RECONSTRUCTION_BUFFER_SIZE);
                        g_receiver_state = STATE_ERROR;
                    }
                    else
                    {
                        APP_PRINT("\r\n[RX] SOT received. Expecting %lu bytes.\r\n", g_expected_file_size);
                        g_received_size = 0;
                        g_next_sequence_no = 0;
                        memset(g_reconstruction_buffer, 0, RECONSTRUCTION_BUFFER_SIZE);
                        g_receiver_state = STATE_RECEIVING;
                    }
                }
                else
                {
                    APP_ERR_PRINT("\r\n[RX] SOT Error: Invalid DLC (%d).\r\n", p_frame->data_length_code);
                    g_receiver_state = STATE_ERROR;
                }
            }
            break;
        }

        case STATE_RECEIVING:
        {
            if (p_frame->id == CAN_ID_FILE_DATA)
            {
                uint16_t received_seq_no;
                memcpy(&received_seq_no, &p_frame->data[0], sizeof(received_seq_no));

                if (received_seq_no == g_next_sequence_no)
                {
                    uint8_t data_len = p_frame->data_length_code - 2;
                    uint32_t offset = g_next_sequence_no * CAN_DATA_MAX_SIZE;

                    if ((offset + data_len) <= g_expected_file_size)
                    {
                        memcpy(&g_reconstruction_buffer[offset], &p_frame->data[2], data_len);
                        g_received_size += data_len;
                        g_next_sequence_no++;
                    }
                    else
                    {
                        APP_ERR_PRINT("\r\n[RX] DATA Error: Received data would overflow buffer.\r\n");
                        g_receiver_state = STATE_ERROR;
                    }
                }
                else
                {
                    APP_ERR_PRINT("\r\n[RX] DATA Error: Out of order packet. Expected %u, got %u.\r\n", g_next_sequence_no, received_seq_no);
                    g_receiver_state = STATE_ERROR;
                }
            }
            else if ((p_frame->id == CAN_ID_FILE_CONTROL) && (p_frame->data[0] == EOT_PACKET))
            {
                if (g_received_size == g_expected_file_size)
                {
                    APP_PRINT("\r\n[RX] EOT received. Transfer successful.\r\n");
                    APP_PRINT("[RX] Reconstructed %lu bytes.\r\n", g_received_size);

                    /* Print the reconstructed content and checksum for verification */
                    APP_PRINT("\r\n[RX] Reconstructed File Content (%lu bytes):\r\n", g_received_size);
                    uint32_t checksum = 0;
                    char line_buffer[128]; // Buffer for one line of hex output
                    int line_char_count = 0;

                    for (uint32_t i = 0; i < g_received_size; i++)
                    {
                        checksum += g_reconstruction_buffer[i];
                        // Add the hex value to the line buffer
                        line_char_count += snprintf(&line_buffer[line_char_count], sizeof(line_buffer) - (size_t)line_char_count, "0x%02X ", g_reconstruction_buffer[i]);

                        // If we've reached 16 bytes or this is the last byte, print the line.
                        if (((i + 1) % 16 == 0) || ((i + 1) == g_received_size))
                        {
                            APP_PRINT("%s\r\n", line_buffer);
                            line_char_count = 0; // Reset for the next line
                            tx_thread_sleep(5); // Give RTT a moment to flush
                        }
                    }
                    APP_PRINT("[RX] Checksum (simple sum): 0x%lX\r\n", checksum);
                    g_receiver_state = STATE_IDLE; // Reset for next transfer
                }
                else
                {
                    APP_ERR_PRINT("\r\n[RX] EOT Error: File size mismatch. Expected %lu, got %lu.\r\n", g_expected_file_size, g_received_size);
                    g_receiver_state = STATE_ERROR;
                }
            }
            break;
        }

        case STATE_FINISHED:
        {
            // This state is no longer used for processing, but we'll keep it to reset to IDLE as a fallback.
            g_receiver_state = STATE_IDLE;
            break;
        }

        case STATE_ERROR:
        {
            // An error occurred. Reset to IDLE.
            APP_ERR_PRINT("[RX] Resetting receiver due to error.\r\n");
            g_receiver_state = STATE_IDLE;
            break;
        }

        default:
        {
            g_receiver_state = STATE_IDLE;
            break;
        }
    }
}

/***********************************************************************************************************************
 * @brief     This function responsible for transmitting the CAN message.
 * @param[IN] can_node_t *p_node, can_frame_t *p_frame
 * @retval    status
 **********************************************************************************************************************/
fsp_err_t can_transmit(can_node_t *p_node, can_frame_t *p_frame)
{
     fsp_err_t status;

     status = R_CAN_Write(p_node->p_ctrl, 0, p_frame);

    return status;
 }
