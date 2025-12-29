/***********************************************************************************************************************
 * File Name    : can.h
 * Description  : Contains data structures and functions setup can module used in can.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef CAN_H_
#define CAN_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define CAN_FRAME_TRANSMIT_DATA_BYTES   (8U)
#define ZERO                            (0U)
#define NULL_CHAR                       ('\0')
#define ONE_MSG                         (1)
#define FOUR_MSG                        (4)
#define REQUIRED_MSG_COUNT              (4)
#define STANDARD_ID_EXAMPLE             (0)
#define EXTENDED_ID_EXAMPLE             (0x12345678)
#define CAN_BUSY_DELAY                  (5000U)

/* CAN File Transfer Protocol Definitions */
#define CAN_ID_FILE_CONTROL             (0x710)
#define CAN_ID_FILE_DATA                (0x711)

typedef enum e_can_file_transfer_opcodes
{
    SOT_PACKET = 0x01, /* Start of Transfer */
    EOT_PACKET = 0x03, /* End of Transfer */
} can_file_transfer_opcodes_t;


#include "can_thread.h"
#include "rtt_thread.h"
#include "filex_thread.h"

#define EP_INFO  "\r\nThis project demonstrates the basic functionalities of CAN running with FIFO support on Renesas RA MCUs\r\n"\
                  "using 2 RA boards. On Board 1 enter any key on RTT viewer to send a remote frame to Board 2 to request data.\r\n"\
                  "When receiving a remote frame, Board 2 will send four messages back-to-back to Board 1. Board 1 receives data\r\n"\
                  "and displays it on the RTT viewer. On successful comparison of data, Board 1 will transmit a data frame to \r\n"\
                  "Board 2 as received acknowledgment. Error messages and information will be printed on the J-link RTT Viewer.\r\n\n"\

typedef struct
{
    /* FSP hardware handles */
    can_instance_ctrl_t * const p_ctrl;
    can_cfg_t const * const     p_cfg;

    /* Node State */
    volatile bool               tx_complete;
    volatile bool               rx_pending;
    volatile bool               error;

    /* Data Buffer */
    can_frame_t                 frame;
    uint8_t                     rx_count;

} can_node_t;

void can_init (void);
fsp_err_t can_transmit(can_node_t *p_node, can_frame_t *p_frame);


/* Inter-thread message queue definitions */
#define CAN_DATA_MAX_SIZE       (6)  /* Max data bytes in a DATA packet */

/* Defines the type of message sent to the CAN thread */
typedef enum e_can_msg_type
{
    CAN_MSG_TYPE_SOT,  /* Start of Transfer */
    CAN_MSG_TYPE_DATA, /* Data chunk */
    CAN_MSG_TYPE_EOT,  /* End of Transfer */
} can_msg_type_t;

/* Defines the message structure for the CAN transmission queue */
typedef struct s_can_msg
{
    can_msg_type_t msg_type;
    union
    {
        /* For SOT message */
        uint32_t file_size;

        /* For DATA message */
        struct
        {
            uint8_t data[CAN_DATA_MAX_SIZE];
            uint8_t length;
            uint16_t sequence_no;
        } data_packet;
    } payload;
} can_msg_t;

extern TX_QUEUE g_can_tx_queue;


#endif /* CAN_H_ */
