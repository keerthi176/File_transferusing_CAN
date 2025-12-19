/***********************************************************************************************************************
 * File Name    : can_app_thread_entry.c
 * Description  : Entry function and implementation for the CAN thread.
 ***********************************************************************************************************************/
#include "filex_thread.h"
#include "common_utils.h"
#include "can.h"
#include <string.h>

/* Define the CAN event flags group. Declared extern in can.h */
TX_EVENT_FLAGS_GROUP g_can_event_flags;


/* CAN Thread entry function */
void can_app_thread_entry(void)
{
    UINT tx_err = TX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;
    ULONG actual_flags = 0;

    /* Create the event flags group */
    tx_err = tx_event_flags_create(&g_can_event_flags, "CAN Event Flags");
    if (TX_SUCCESS != tx_err)
    {
        APP_ERR_PRINT("\r\nFailed to create CAN event flags.\r\n");
        APP_ERR_TRAP(tx_err);
    }

    /* Initialize both CAN channels in Normal Mode */
    can_init();

    APP_PRINT("\r\nCAN Thread started. Waiting for user to press 5...\r\n");

    while (1)
    {
        /* Wait for any CAN event */
        tx_err = tx_event_flags_get(&g_can_event_flags,
                                    (CAN_EVENT_FLAG_RX_CAN0 | CAN_EVENT_FLAG_RX_CAN1 | CAN_EVENT_FLAG_TX_COMPLETE | CAN_EVENT_FLAG_ERR | CAN_EVENT_FLAG_START_TRANSFER),
                                    TX_OR_CLEAR,
                                    &actual_flags,
                                    TX_WAIT_FOREVER);

        if (TX_SUCCESS == tx_err)
        {
            if (actual_flags & CAN_EVENT_FLAG_RX_CAN1)
            {
                APP_PRINT("\r\nSUCCESS: Message received on CAN1!\r\n");
            }
            if (actual_flags & CAN_EVENT_FLAG_RX_CAN0)
            {
                APP_PRINT("\r\nINFO: Message received on CAN0.\r\n");
            }
            if (actual_flags & CAN_EVENT_FLAG_TX_COMPLETE)
            {
                APP_PRINT("\r\nINFO: TX complete event received.\r\n");
            }
            if (actual_flags & CAN_EVENT_FLAG_ERR)
            {
                APP_ERR_PRINT("\r\nERROR: CAN Error event received.\r\n");
            }
            if (actual_flags & CAN_EVENT_FLAG_START_TRANSFER)
            {
                APP_PRINT("\r\nINFO: Start event received. Sending 'Hello' from CAN0 to CAN1...\r\n");

                can_frame_t frame;
                memset(&frame, 0, sizeof(can_frame_t));

                frame.id = ID_HELLO_WORLD; // ID is 0x100
                frame.id_mode = CAN_ID_MODE_STANDARD;
                frame.type = CAN_FRAME_TYPE_DATA;
                frame.data_length_code = 8;
                memcpy(frame.data, "Hello W", 8);

                /* Transmit on CAN0 using the g_can_nodes array from can.c */
                err = can_transmit(&g_can_nodes[0], &frame);

                if(FSP_SUCCESS == err)
                {
                    APP_PRINT("\r\nINFO: 'Hello' message write to CAN0 successful.\r\n");
                }
                else
                {
                    APP_ERR_PRINT("\r\nERROR: Failed to write 'Hello' message to CAN0.\r\n");
                    APP_ERR_TRAP(err);
                }
            }
        }
    }
}

