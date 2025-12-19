/***********************************************************************************************************************
 * File Name    : can.c
 * Description  : Contains data structures and functions to setup CAN module.
 **********************************************************************************************************************/
#include "can.h"
#include "common_utils.h"
#include "tx_api.h"
#include "can_app_thread.h"

/* Extern the event flags group defined in can_app_thread_entry.c */
extern TX_EVENT_FLAGS_GROUP g_can_event_flags;

/* Local copies of FSP configurations */
static can_cfg_t g_can0_cfg_local;
static can_cfg_t g_can1_cfg_local;

/* Array of nodes to manage CAN channels, adapted from user example */
can_node_t g_can_nodes[2] =
{
    [0] = { .p_ctrl = &g_can0_ctrl, .p_cfg = &g_can0_cfg_local },
    [1] = { .p_ctrl = &g_can1_ctrl, .p_cfg = &g_can1_cfg_local }
};

static void can_handle_error(fsp_err_t err,  const char * err_str);

/*******************************************************************************************************************//**
 * @brief       Initializes both CAN channels in Normal Mode.
 **********************************************************************************************************************/
void can_init(void)
{
    fsp_err_t status = FSP_SUCCESS;

    /* --- Initialize CAN0 --- */
    g_can0_cfg_local = g_can0_cfg;
    g_can0_cfg_local.p_callback = can_callback;
    g_can0_cfg_local.p_context = &g_can_nodes[0];
    status = R_CAN_Open(g_can_nodes[0].p_ctrl, g_can_nodes[0].p_cfg);
    can_handle_error(status, "\r\n **CAN0 Open API failed**");
    APP_PRINT("\r\nCAN0 Opened Successfully in Normal Mode.\r\n");

    /* --- Initialize CAN1 --- */
    g_can1_cfg_local = g_can1_cfg;
    g_can1_cfg_local.p_callback = can_callback;
    g_can1_cfg_local.p_context = &g_can_nodes[1];
    status = R_CAN_Open(g_can_nodes[1].p_ctrl, g_can_nodes[1].p_cfg);
    can_handle_error(status, "\r\n **CAN1 Open API failed**");
    APP_PRINT("\r\nCAN1 Opened Successfully in Normal Mode.\r\n");
}

/*******************************************************************************************************************//**
 * @brief       Transmits a CAN frame on a specific node.
 **********************************************************************************************************************/
fsp_err_t can_transmit(can_node_t *p_node, can_frame_t *p_frame)
{
     /* The working example uses Mailbox 4, which likely targets the FIFO. */
     return R_CAN_Write(p_node->p_ctrl, 4, p_frame);
}

/*******************************************************************************************************************//**
 * @brief       This function is the CAN callback for both channels.
 **********************************************************************************************************************/
void can_callback(can_callback_args_t *p_args)
{
    switch(p_args->event)
    {
        case CAN_EVENT_TX_COMPLETE:
        {
            tx_event_flags_set(&g_can_event_flags, CAN_EVENT_FLAG_TX_COMPLETE, TX_OR);
            break;
        }

        case CAN_EVENT_RX_COMPLETE:
        {
            if (0 == p_args->channel)
            {
                tx_event_flags_set(&g_can_event_flags, CAN_EVENT_FLAG_RX_CAN0, TX_OR);
            }
            else if (1 == p_args->channel)
            {
                tx_event_flags_set(&g_can_event_flags, CAN_EVENT_FLAG_RX_CAN1, TX_OR);
            }
            break;
        }

        case CAN_EVENT_ERR_WARNING:
        case CAN_EVENT_ERR_PASSIVE:
        case CAN_EVENT_ERR_BUS_OFF:
        case CAN_EVENT_FIFO_MESSAGE_LOST:
        {
            tx_event_flags_set(&g_can_event_flags, CAN_EVENT_FLAG_ERR, TX_OR);
            break;
        }

        default:
        {
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function handles errors and de-initializes the CAN modules.
 **********************************************************************************************************************/
static void can_handle_error(fsp_err_t err,  const char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT(err_str);
        if(g_can0_ctrl.open)
        {
            R_CAN_Close (&g_can0_ctrl);
        }
        if(g_can1_ctrl.open)
        {
            R_CAN_Close (&g_can1_ctrl);
        }
        APP_ERR_TRAP(err);
    }
}
