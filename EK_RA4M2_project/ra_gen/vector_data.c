/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = fcu_frdyi_isr, /* FCU FRDYI (Flash ready interrupt) */
            [1] = fcu_fiferr_isr, /* FCU FIFERR (Flash access error interrupt) */
            [2] = can_error_isr, /* CAN0 ERROR (Error interrupt) */
            [3] = can_rx_isr, /* CAN0 MAILBOX RX (Reception complete interrupt) */
            [4] = can_tx_isr, /* CAN0 MAILBOX TX (Transmission complete interrupt) */
            [5] = can_rx_isr, /* CAN0 FIFO RX (Receive FIFO interrupt) */
            [6] = can_tx_isr, /* CAN0 FIFO TX (Transmit FIFO interrupt) */
            [7] = sci_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [8] = sci_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [9] = sci_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [10] = sci_uart_eri_isr, /* SCI0 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_FCU_FRDYI,GROUP0), /* FCU FRDYI (Flash ready interrupt) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_FCU_FIFERR,GROUP1), /* FCU FIFERR (Flash access error interrupt) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_CAN0_ERROR,GROUP2), /* CAN0 ERROR (Error interrupt) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_CAN0_MAILBOX_RX,GROUP3), /* CAN0 MAILBOX RX (Reception complete interrupt) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_CAN0_MAILBOX_TX,GROUP4), /* CAN0 MAILBOX TX (Transmission complete interrupt) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_CAN0_FIFO_RX,GROUP5), /* CAN0 FIFO RX (Receive FIFO interrupt) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_CAN0_FIFO_TX,GROUP6), /* CAN0 FIFO TX (Transmit FIFO interrupt) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI0_RXI,GROUP7), /* SCI0 RXI (Receive data full) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TXI,GROUP0), /* SCI0 TXI (Transmit data empty) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TEI,GROUP1), /* SCI0 TEI (Transmit end) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI0_ERI,GROUP2), /* SCI0 ERI (Receive error) */
        };
        #endif
        #endif
