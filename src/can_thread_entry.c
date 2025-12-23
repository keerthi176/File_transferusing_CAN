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

/* CAN_Thread entry function */
void can_thread_entry(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    R_FSP_VersionGet(&version);

    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
//    APP_PRINT(EP_INFO);

//    tx_thread_resume(&can_thread);

    //initialise the can bus.
    can_init();

    while (1)
    {
        if(CAN_Flag == true)
        {
            // Initialize the variable.
            can_frame_t hello_frame = {0};
            hello_frame.id = ID_HELLO_WORLD;
            hello_frame.id_mode = CAN_ID_MODE_STANDARD;
            hello_frame.type = CAN_FRAME_TYPE_DATA;
            hello_frame.data_length_code = 8;
            memcpy(hello_frame.data, "Hello W", 8);

                fsp_err_t status = can_transmit(&g_can_nodes[0], &hello_frame);
                if (FSP_SUCCESS != status)
                {
                    APP_PRINT("\r\n[ERR] In can_run(): CAN0 Transmit failed! Error code: %d", status);
                }

                for (uint8_t i = 0; i < 2; i++)
                {
                    if (g_can_nodes[i].rx_pending)
                    {
                        g_can_nodes[i].rx_pending = false;
                        process_message(&g_can_nodes[i]);
                    }
                    if (g_can_nodes[i].error)
                    {
                        g_can_nodes[i].error = false;
                        APP_ERR_PRINT("\r\nCAN Bus Error on Channel %d!", g_can_nodes[i].p_cfg->channel);
                    }
                 }

                tx_thread_sleep (10);
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
    APP_PRINT("\r\n[Ch%d RX] ID: 0x%lX, DLC: %d, Data: '%.*s'",
              p_node->p_cfg->channel,
              p_node->frame.id,
              p_node->frame.data_length_code,
              p_node->frame.data_length_code,
              (char*)p_node->frame.data);
}

fsp_err_t can_transmit(can_node_t *p_node, can_frame_t *p_frame)
{
     fsp_err_t status;

     status = R_CAN_Write(p_node->p_ctrl, 0, p_frame);

    return status;
 }
