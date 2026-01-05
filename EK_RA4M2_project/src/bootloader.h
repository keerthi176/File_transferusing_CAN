#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "hal_data.h"

/* The firmware buffer is used by both CAN and UART modes. Let's make it 64KB. */
#define FIRMWARE_BUFFER_SIZE        (64 * 1024)
extern uint8_t g_firmware_buffer[FIRMWARE_BUFFER_SIZE];

/* Function Prototypes */
void bootloader_init(void);
void can_callback(can_callback_args_t *p_args);
void flash_callback(flash_callback_args_t *p_args);
void bootloader_jump_to_app(void);
void bootloader_uart_mode(void);
void uart_callback(uart_callback_args_t *p_args);

/* Shared Definitions */
#define APP_VALID_MARKER            (0x55AA55AA)
#define APP_FIRMWARE_START_ADDRESS  (0x10000)

typedef enum e_receiver_state
{
    STATE_IDLE,
    STATE_RECEIVING,
    STATE_COMPLETE,
    STATE_ERROR
} receiver_state_t;

/* CAN Packet Definitions */
#define CAN_ID_FILE_CONTROL (0x100)
#define CAN_ID_FILE_DATA    (0x101)
#define CAN_DATA_MAX_SIZE   (6)
typedef enum e_packet_type
{
    SOT_PACKET = 1,
    DATA_PACKET = 2,
    EOT_PACKET = 3,
} packet_type_t;

#endif /* BOOTLOADER_H_ */
