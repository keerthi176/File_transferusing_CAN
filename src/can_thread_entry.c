#include "can_thread.h"
#include "can.h"
#include "common_utils.h"

#define ID_HELLO_WORLD      (0x100)

static can_cfg_t g_can0_cfg_local;
static can_cfg_t g_can1_cfg_local;
extern bool CAN_Flag;

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

/* CAN Thread entry function */
/* DEFINE THE QUEUE AND BUFFER */
#define CAN_QUEUE_MAX_MESSAGES  (64)

TX_QUEUE g_can_tx_queue;
// The queue needs a buffer to store the messages (which are pointers).
// Each message pointer is 1 ULONG (4 bytes).
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
//    APP_PRINT(EP_INFO);

//    tx_thread_resume(&can_thread);

#define CAN_TX_TIMEOUT_MS (100)

    //initialise the can bus.
    can_init();

    while (1)
    {
		UINT tx_status;
		can_msg_t *p_can_msg = NULL;

		// Try to receive a message from the queue without blocking
		tx_status = tx_queue_receive(&g_can_tx_queue, (VOID *)&p_can_msg, TX_NO_WAIT);

		if (TX_SUCCESS == tx_status)
		{
			// Message received, process and send it
			can_frame_t frame;
			memset(&frame, 0, sizeof(can_frame_t));
			frame.id_mode = CAN_ID_MODE_STANDARD;
			frame.type = CAN_FRAME_TYPE_DATA;

			fsp_err_t fsp_status = FSP_SUCCESS;

			// Construct the CAN frame based on the message type
			switch (p_can_msg->msg_type)
			{
				case CAN_MSG_TYPE_SOT:
				{
					APP_PRINT("[CONSUMER] Sending SOT. File size: %lu\r\n", p_can_msg->payload.file_size);
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
					APP_PRINT("[CONSUMER] Sending EOT.\r\n");
					frame.id = CAN_ID_FILE_CONTROL;
					frame.data_length_code = 1;
					frame.data[0] = EOT_PACKET;
					break;
				}

				default:
					APP_PRINT("[CONSUMER] Received unknown message type.\r\n");
					tx_byte_release(p_can_msg); // Release memory and ignore
					continue; // Skip to next message
			}

			// Transmit the prepared frame
			g_can_nodes[0].tx_complete = false; // Reset flag before transmit
			fsp_status = R_CAN_Write(g_can_nodes[0].p_ctrl, 0, &frame);
			if (FSP_SUCCESS != fsp_status)
			{
				APP_ERR_PRINT("[CONSUMER] R_CAN_Write failed with status: %d\r\n", fsp_status);
			}
			else
			{
				// Wait for TX complete flag to be set by the callback, with a timeout
				UINT wait_count = 0;
				while(false == g_can_nodes[0].tx_complete)
				{
					tx_thread_sleep(1);
					wait_count++;
					if (wait_count > CAN_TX_TIMEOUT_MS)
					{
						 APP_ERR_PRINT("[CONSUMER] CAN TX timeout!\r\n");
						 break;
					}
				}
			}

			// Release the memory block back to the pool now that it has been sent
			tx_byte_release(p_can_msg);
		}


		// Always check for and clear any pending RX messages or errors to prevent lockup in loopback mode
		if (g_can_nodes[1].rx_pending)
		{
			g_can_nodes[1].rx_pending = false;
			// Process the received frame to print its contents
			process_message(&g_can_nodes[1]);
		}
		if (g_can_nodes[1].error)
		{
			g_can_nodes[1].error = false;
			APP_ERR_PRINT("\r\n[CONSUMER] CAN Bus Error on Channel %d!\r\n", g_can_nodes[1].p_cfg->channel);
		}

		// If no message was received, give other threads time to run
		if (TX_QUEUE_EMPTY == tx_status)
		{
			tx_thread_sleep(5); // Sleep for 5 ticks if queue is empty
		}
    }
}

void can_init(void)
{
    fsp_err_t status = FSP_SUCCESS;

    g_can0_cfg_local = g_can0_cfg;
    g_can0_cfg_local.p_callback = can_callback;
    g_can0_cfg_local.p_context = &g_can_nodes[0];

    status = R_CAN_Open(g_can_nodes[0].p_ctrl, g_can_nodes[0].p_cfg);
    can_handle_error(status, "\r\n **CAN0 Open API failed**");

    status = R_CAN_ModeTransition(g_can_nodes[0].p_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_LOOPBACK_EXTERNAL);
    can_handle_error(status, "\r\n **CAN0 External Loopback transition failed**");
    APP_PRINT("\r\nCAN0 configured for External Loopback Mode to enable self-reception.\r\n");

    g_can1_cfg_local = g_can1_cfg;
    g_can1_cfg_local.p_callback = can_callback;
    g_can1_cfg_local.p_context = &g_can_nodes[1];

    status = R_CAN_Open(g_can_nodes[1].p_ctrl, g_can_nodes[1].p_cfg);
    can_handle_error(status, "\r\n **CAN1 Open API failed**");
}

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

void process_message(can_node_t *p_node)
{
    APP_PRINT("\r\n[CAN%d RX] ID: 0x%lX, DLC: %d, Data: ",
              p_node->p_cfg->channel,
              p_node->frame.id,
              p_node->frame.data_length_code);

    for (int i = 0; i < p_node->frame.data_length_code; i++)
    {
        APP_PRINT("0x%02X ", p_node->frame.data[i]);
    }
}

fsp_err_t can_transmit(can_node_t *p_node, can_frame_t *p_frame)
{
     fsp_err_t status;

     status = R_CAN_Write(p_node->p_ctrl, 0, p_frame);

    return status;
 }
