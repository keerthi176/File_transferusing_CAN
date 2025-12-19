/***********************************************************************************************************************
 * File Name    : can.h
 * Description  : Contains data structures and functions for the CAN driver.
 **********************************************************************************************************************/
#ifndef CAN_H_
#define CAN_H_

#include "hal_data.h"
#include "r_can.h"

/* Event flags for CAN communication, used by the CAN thread */
#define CAN_EVENT_FLAG_TX_COMPLETE      (1 << 0)
#define CAN_EVENT_FLAG_RX_CAN0          (1 << 1)
#define CAN_EVENT_FLAG_RX_CAN1          (1 << 2)
#define CAN_EVENT_FLAG_ERR              (1 << 3)
#define CAN_EVENT_FLAG_START_TRANSFER   (1 << 4) // Used by filex_thread to trigger the CAN thread

/* CAN ID for the 'Hello' message test */
#define ID_HELLO_WORLD      (0x100)

/* Simple node structure to hold state, adapted from user example */
typedef struct
{
    can_instance_ctrl_t * const p_ctrl;
    can_cfg_t const *           p_cfg; // Made non-const to allow modification
    can_frame_t                 frame;
} can_node_t;

/* Global array of CAN nodes, defined in can.c */
extern can_node_t g_can_nodes[2];

/* Function Prototypes */
void can_init(void);
fsp_err_t can_transmit(can_node_t *p_node, can_frame_t *p_frame);
void can_callback(can_callback_args_t *p_args);

#endif /* CAN_H_ */
