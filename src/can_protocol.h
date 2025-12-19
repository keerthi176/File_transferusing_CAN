/***********************************************************************************************************************
 * File Name    : can_app_thread.h
 * Description  : Declarations for the CAN Application thread.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAN_PROTOCOL_H_
#define CAN_PROTOCOL_H_

#include "tx_api.h"

/* Event flags for CAN communication */
#define CAN_EVENT_FLAG_RX               (1 << 0)    // Event flag for CAN message reception
#define CAN_EVENT_FLAG_TX               (1 << 1)    // Event flag for CAN message transmission complete
#define CAN_EVENT_FLAG_ERR              (1 << 2)    // Event flag for CAN error
#define CAN_EVENT_FLAG_START_TRANSFER   (1 << 3)    // Event flag to start the file transfer

/* Extern declaration for the event flags group used across CAN files */
extern TX_EVENT_FLAGS_GROUP g_can_event_flags;

#endif /* CAN_PROTOCOL_H_ */
