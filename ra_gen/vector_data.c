/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [1] = sci_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI0 ERI (Receive error) */
            [4] = can_error_isr, /* CAN0 ERROR (Error interrupt) */
            [5] = can_rx_isr, /* CAN0 MAILBOX RX (Reception complete interrupt) */
            [6] = can_tx_isr, /* CAN0 MAILBOX TX (Transmission complete interrupt) */
            [7] = can_rx_isr, /* CAN0 FIFO RX (Receive FIFO interrupt) */
            [8] = can_tx_isr, /* CAN0 FIFO TX (Transmit FIFO interrupt) */
            [9] = can_error_isr, /* CAN1 ERROR (Error interrupt) */
            [10] = can_rx_isr, /* CAN1 MAILBOX RX (Reception complete interrupt) */
            [11] = can_tx_isr, /* CAN1 MAILBOX TX (Transmission complete interrupt) */
            [12] = can_rx_isr, /* CAN1 FIFO RX (Receive FIFO interrupt) */
            [13] = can_tx_isr, /* CAN1 FIFO TX (Transmit FIFO interrupt) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SCI0_RXI,GROUP0), /* SCI0 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TXI,GROUP1), /* SCI0 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TEI,GROUP2), /* SCI0 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI0_ERI,GROUP3), /* SCI0 ERI (Receive error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_CAN0_ERROR,GROUP4), /* CAN0 ERROR (Error interrupt) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_CAN0_MAILBOX_RX,GROUP5), /* CAN0 MAILBOX RX (Reception complete interrupt) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_CAN0_MAILBOX_TX,GROUP6), /* CAN0 MAILBOX TX (Transmission complete interrupt) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_CAN0_FIFO_RX,GROUP7), /* CAN0 FIFO RX (Receive FIFO interrupt) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_CAN0_FIFO_TX,GROUP0), /* CAN0 FIFO TX (Transmit FIFO interrupt) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_CAN1_ERROR,GROUP1), /* CAN1 ERROR (Error interrupt) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_CAN1_MAILBOX_RX,GROUP2), /* CAN1 MAILBOX RX (Reception complete interrupt) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_CAN1_MAILBOX_TX,GROUP3), /* CAN1 MAILBOX TX (Transmission complete interrupt) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_CAN1_FIFO_RX,GROUP4), /* CAN1 FIFO RX (Receive FIFO interrupt) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_CAN1_FIFO_TX,GROUP5), /* CAN1 FIFO TX (Transmit FIFO interrupt) */
        };
        #endif
        #endif
