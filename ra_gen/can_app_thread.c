/* generated thread source file - do not edit */
#include "can_app_thread.h"

TX_THREAD can_app_thread;
void can_app_thread_create(void);
static void can_app_thread_func(ULONG thread_input);
static uint8_t can_app_thread_stack[1024] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.can_app_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#ifndef CAN1_BAUD_SETTINGS_OVERRIDE
#define CAN1_BAUD_SETTINGS_OVERRIDE  (1)
#endif
#if CAN1_BAUD_SETTINGS_OVERRIDE
can_bit_timing_cfg_t g_can1_bit_timing_cfg =
{ .baud_rate_prescaler = 4, .time_segment_1 = 10, .time_segment_2 = 4, .synchronization_jump_width = 4 };
#else
can_bit_timing_cfg_t g_can1_bit_timing_cfg =
{
    /* Actual bitrate: 500000 Hz. Actual Bit Time Ratio: 75 %. */  .baud_rate_prescaler = 1 +2 /* Division value of baud rate prescaler */, .time_segment_1 = 11, .time_segment_2 = 4, .synchronization_jump_width = 4,
};
#endif

uint32_t g_can1_mailbox_mask[CAN_NO_OF_MAILBOXES_g_can1 / 4] =
{ 0x1FFFFFFF,
#if CAN_NO_OF_MAILBOXES_g_can1 > 4
0x1FFFFFFF,
#endif
#if CAN_NO_OF_MAILBOXES_g_can1 > 8
0x1FFFFFFF,
0x1FFFFFFF,
#endif
#if CAN_NO_OF_MAILBOXES_g_can1 > 16
0x1FFFFFFF,
0x1FFFFFFF,
#endif
#if CAN_NO_OF_MAILBOXES_g_can1 > 24
0x1FFFFFFF,
0x1FFFFFFF,
#endif
        };

can_mailbox_t g_can1_mailbox[CAN_NO_OF_MAILBOXES_g_can1] =
{
{ .mailbox_id = 32, .id_mode = CAN_ID_MODE_STANDARD, .mailbox_type = CAN_MAILBOX_TRANSMIT, .frame_type =
          CAN_FRAME_TYPE_REMOTE },
  { .mailbox_id = 33, .id_mode = CAN_ID_MODE_STANDARD, .mailbox_type = CAN_MAILBOX_RECEIVE, .frame_type =
            CAN_FRAME_TYPE_DATA },
  { .mailbox_id = 256, .id_mode = CAN_ID_MODE_STANDARD, .mailbox_type = CAN_MAILBOX_RECEIVE, .frame_type =
            CAN_FRAME_TYPE_DATA, },
  { .mailbox_id = 258, .id_mode = CAN_ID_MODE_STANDARD, .mailbox_type = CAN_MAILBOX_TRANSMIT, .frame_type =
            CAN_FRAME_TYPE_DATA },
#if CAN_NO_OF_MAILBOXES_g_can1 > 4
    {
        .mailbox_id              =  4,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  5,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  6,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  7,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
#endif
#if CAN_NO_OF_MAILBOXES_g_can1 > 8
    {
        .mailbox_id              =  8,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  9,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  10,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  11,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  12,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA,
    },
    {
        .mailbox_id              =  13,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  14,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  15,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
#endif
#if CAN_NO_OF_MAILBOXES_g_can1 > 16
    {
        .mailbox_id              =  16,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },

    {
        .mailbox_id              =  17,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  18,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  19,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  20,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  21,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  22,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA,
    },
    {
        .mailbox_id              =  23,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
#endif
#if CAN_NO_OF_MAILBOXES_g_can1 > 24
    {
        .mailbox_id              =  24,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  25,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  26,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  27,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  28,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  29,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  30,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  31,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    }
#endif
        };

#if CAN_CFG_FIFO_SUPPORT
const can_fifo_interrupt_cfg_t g_can1_fifo_int_cfg =
{
    .fifo_int_mode           = (can_fifo_interrupt_mode_t) (CAN_FIFO_INTERRUPT_MODE_RX_EVERY_FRAME | CAN_FIFO_INTERRUPT_MODE_TX_EMPTY),
    .rx_fifo_irq             = VECTOR_NUMBER_CAN1_FIFO_RX,
    .tx_fifo_irq             = VECTOR_NUMBER_CAN1_FIFO_TX,
};

can_rx_fifo_cfg_t g_can1_rx_fifo_cfg =
{
    .rx_fifo_mask1 = 0x1FFFFFFF,
    .rx_fifo_mask2 = 0x1FFFFFFF,

    .rx_fifo_id1 =
    {
        .mailbox_id              =  0,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_REMOTE
    },

    .rx_fifo_id2 =
    {
        .mailbox_id              =  0,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_REMOTE
    },
};
#endif

const can_extended_cfg_t g_can1_extended_cfg =
{ .clock_source = CAN_CLOCK_SOURCE_CANMCLK,
  .p_mailbox_mask = g_can1_mailbox_mask,
  .p_mailbox = g_can1_mailbox,
  .global_id_mode = CAN_GLOBAL_ID_MODE_STANDARD,
  .mailbox_count = CAN_NO_OF_MAILBOXES_g_can1,
  .message_mode = CAN_MESSAGE_MODE_OVERRUN,
#if CAN_CFG_FIFO_SUPPORT
    .p_fifo_int_cfg         = &g_can1_fifo_int_cfg,
    .p_rx_fifo_cfg          = &g_can1_rx_fifo_cfg,
#else
  .p_fifo_int_cfg = NULL,
  .p_rx_fifo_cfg = NULL,
#endif
        };

can_instance_ctrl_t g_can1_ctrl;
const can_cfg_t g_can1_cfg =
{ .channel = 1,
  .p_bit_timing = &g_can1_bit_timing_cfg,
  .p_callback = can_callback,
  .p_extend = &g_can1_extended_cfg,
  .p_context = NULL,
  .ipl = (12),
#if defined(VECTOR_NUMBER_CAN1_MAILBOX_TX)
    .tx_irq             = VECTOR_NUMBER_CAN1_MAILBOX_TX,
#else
  .tx_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_CAN1_MAILBOX_RX)
    .rx_irq             = VECTOR_NUMBER_CAN1_MAILBOX_RX,
#else
  .rx_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_CAN1_ERROR)
    .error_irq             = VECTOR_NUMBER_CAN1_ERROR,
#else
  .error_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const can_instance_t g_can1 =
{ .p_ctrl = &g_can1_ctrl, .p_cfg = &g_can1_cfg, .p_api = &g_can_on_can };
#ifndef CAN0_BAUD_SETTINGS_OVERRIDE
#define CAN0_BAUD_SETTINGS_OVERRIDE  (1)
#endif
#if CAN0_BAUD_SETTINGS_OVERRIDE
can_bit_timing_cfg_t g_can0_bit_timing_cfg =
{ .baud_rate_prescaler = 4, .time_segment_1 = 10, .time_segment_2 = 4, .synchronization_jump_width = 4 };
#else
can_bit_timing_cfg_t g_can0_bit_timing_cfg =
{
    /* Actual bitrate: 500000 Hz. Actual Bit Time Ratio: 75 %. */  .baud_rate_prescaler = 1 +2 /* Division value of baud rate prescaler */, .time_segment_1 = 11, .time_segment_2 = 4, .synchronization_jump_width = 4,
};
#endif

uint32_t g_can0_mailbox_mask[CAN_NO_OF_MAILBOXES_g_can0 / 4] =
{ 0x1FFFFFFF,
#if CAN_NO_OF_MAILBOXES_g_can0 > 4
0x1FFFFFFF,
#endif
#if CAN_NO_OF_MAILBOXES_g_can0 > 8
0x1FFFFFFF,
0x1FFFFFFF,
#endif
#if CAN_NO_OF_MAILBOXES_g_can0 > 16
0x1FFFFFFF,
0x1FFFFFFF,
#endif
#if CAN_NO_OF_MAILBOXES_g_can0 > 24
0x1FFFFFFF,
0x1FFFFFFF,
#endif
        };

can_mailbox_t g_can0_mailbox[CAN_NO_OF_MAILBOXES_g_can0] =
{
{ .mailbox_id = 32, .id_mode = CAN_ID_MODE_STANDARD, .mailbox_type = CAN_MAILBOX_TRANSMIT, .frame_type =
          CAN_FRAME_TYPE_REMOTE },
  { .mailbox_id = 33, .id_mode = CAN_ID_MODE_STANDARD, .mailbox_type = CAN_MAILBOX_RECEIVE, .frame_type =
            CAN_FRAME_TYPE_DATA },
  { .mailbox_id = 256, .id_mode = CAN_ID_MODE_STANDARD, .mailbox_type = CAN_MAILBOX_RECEIVE, .frame_type =
            CAN_FRAME_TYPE_DATA, },
  { .mailbox_id = 258, .id_mode = CAN_ID_MODE_STANDARD, .mailbox_type = CAN_MAILBOX_RECEIVE, .frame_type =
            CAN_FRAME_TYPE_DATA },
#if CAN_NO_OF_MAILBOXES_g_can0 > 4
    {
        .mailbox_id              =  4,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  5,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  6,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  7,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
#endif
#if CAN_NO_OF_MAILBOXES_g_can0 > 8
    {
        .mailbox_id              =  8,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  9,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  10,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  11,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  12,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA,
    },
    {
        .mailbox_id              =  13,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  14,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  15,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
#endif
#if CAN_NO_OF_MAILBOXES_g_can0 > 16
    {
        .mailbox_id              =  16,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },

    {
        .mailbox_id              =  17,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  18,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  19,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  20,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  21,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  22,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA,
    },
    {
        .mailbox_id              =  23,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
#endif
#if CAN_NO_OF_MAILBOXES_g_can0 > 24
    {
        .mailbox_id              =  24,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  25,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  26,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  27,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  28,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  29,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  30,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },
    {
        .mailbox_id              =  31,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    }
#endif
        };

#if CAN_CFG_FIFO_SUPPORT
const can_fifo_interrupt_cfg_t g_can0_fifo_int_cfg =
{
    .fifo_int_mode           = (can_fifo_interrupt_mode_t) (CAN_FIFO_INTERRUPT_MODE_RX_EVERY_FRAME | CAN_FIFO_INTERRUPT_MODE_TX_EMPTY),
    .rx_fifo_irq             = VECTOR_NUMBER_CAN0_FIFO_RX,
    .tx_fifo_irq             = VECTOR_NUMBER_CAN0_FIFO_TX,
};

can_rx_fifo_cfg_t g_can0_rx_fifo_cfg =
{
    .rx_fifo_mask1 = 0x7FC,
    .rx_fifo_mask2 = 0x1FFFFFFF,

    .rx_fifo_id1 =
    {
        .mailbox_id              =  0,
        .id_mode                 =  CAN_ID_MODE_STANDARD,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_DATA
    },

    .rx_fifo_id2 =
    {
        .mailbox_id              =  0x12345678,
        .id_mode                 =  CAN_ID_MODE_EXTENDED,
        .mailbox_type            =  CAN_MAILBOX_RECEIVE,
        .frame_type              =  CAN_FRAME_TYPE_REMOTE
    },
};
#endif

const can_extended_cfg_t g_can0_extended_cfg =
{ .clock_source = CAN_CLOCK_SOURCE_CANMCLK,
  .p_mailbox_mask = g_can0_mailbox_mask,
  .p_mailbox = g_can0_mailbox,
  .global_id_mode = CAN_GLOBAL_ID_MODE_STANDARD,
  .mailbox_count = CAN_NO_OF_MAILBOXES_g_can0,
  .message_mode = CAN_MESSAGE_MODE_OVERRUN,
#if CAN_CFG_FIFO_SUPPORT
    .p_fifo_int_cfg         = &g_can0_fifo_int_cfg,
    .p_rx_fifo_cfg          = &g_can0_rx_fifo_cfg,
#else
  .p_fifo_int_cfg = NULL,
  .p_rx_fifo_cfg = NULL,
#endif
        };

can_instance_ctrl_t g_can0_ctrl;
const can_cfg_t g_can0_cfg =
{ .channel = 0,
  .p_bit_timing = &g_can0_bit_timing_cfg,
  .p_callback = can_callback,
  .p_extend = &g_can0_extended_cfg,
  .p_context = NULL,
  .ipl = (12),
#if defined(VECTOR_NUMBER_CAN0_MAILBOX_TX)
    .tx_irq             = VECTOR_NUMBER_CAN0_MAILBOX_TX,
#else
  .tx_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_CAN0_MAILBOX_RX)
    .rx_irq             = VECTOR_NUMBER_CAN0_MAILBOX_RX,
#else
  .rx_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_CAN0_ERROR)
    .error_irq             = VECTOR_NUMBER_CAN0_ERROR,
#else
  .error_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const can_instance_t g_can0 =
{ .p_ctrl = &g_can0_ctrl, .p_cfg = &g_can0_cfg, .p_api = &g_can_on_can };
extern bool g_fsp_common_initialized;
extern uint32_t g_fsp_common_thread_count;
extern TX_SEMAPHORE g_fsp_common_initialized_semaphore;

void can_app_thread_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_fsp_common_thread_count++;

    /* Initialize each kernel object. */

    UINT err;
    err = tx_thread_create (&can_app_thread, (CHAR*) "CAN_Thread", can_app_thread_func, (ULONG) NULL,
                            &can_app_thread_stack, 1024, 1, 1, 1, TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&can_app_thread, 0);
    }
}

static void can_app_thread_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    FSP_PARAMETER_NOT_USED (thread_input);

    /* Initialize common components */
    tx_startup_common_init ();

    /* Initialize each module instance. */

    /* Enter user code for this thread. */
    can_app_thread_entry ();
}
