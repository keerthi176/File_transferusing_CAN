#include "bootloader.h"
#include "common_utils.h"
#include "bsp_api.h"

#define CODE_FLASH_BLOCK_SIZE (0x2000)
#define FLASH_WRITE_SIZE      (128)

/* Shared state machine and buffers */
static receiver_state_t g_receiver_state = STATE_IDLE;
uint8_t g_firmware_buffer[FIRMWARE_BUFFER_SIZE];
static uint32_t g_received_size = 0;
static uint32_t g_expected_file_size = 0;

/* CAN specific variables */
static uint16_t g_can_next_sequence_no = 0;
volatile bool g_transfer_complete = false;

/* UART specific variables */
#define UART_RX_LINE_BUFFER_SIZE (128)
static uint8_t  g_uart_line_buffer[UART_RX_LINE_BUFFER_SIZE];
static uint32_t g_uart_line_idx      = 0;
static uint32_t g_hex_base_address   = 0;
static bool     g_app_erased         = false;

/* This function will be placed in and executed from RAM */
static fsp_err_t flash_firmware_from_ram(uint32_t total_size);

void bootloader_init(void)
{
    SEGGER_RTT_Init();
    SEGGER_RTT_printf(0, "[BOOTLOADER] Starting...\r\n");

    /* Initialize the CAN driver for CAN mode */
    fsp_err_t err = R_CAN_Open(&g_can0_ctrl, &g_can0_cfg);
    if (FSP_SUCCESS != err)
    {
        SEGGER_RTT_printf(0, "[BOOTLOADER] R_CAN_Open failed: %d\r\n", err);
    }
}

/***********************************************************************************************************************
 * Flashing Function (to be executed from RAM)
 **********************************************************************************************************************/
__attribute__((section(".ramfunc")))
static fsp_err_t flash_firmware_from_ram(uint32_t total_size)
{
    fsp_err_t err;

    err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    uint32_t num_blocks = (total_size + (CODE_FLASH_BLOCK_SIZE - 1)) / CODE_FLASH_BLOCK_SIZE;
    err = R_FLASH_HP_Erase(&g_flash0_ctrl, APP_FIRMWARE_START_ADDRESS, num_blocks);
    if (FSP_SUCCESS != err)
    {
        R_FLASH_HP_Close(&g_flash0_ctrl);
        return err;
    }

    uint32_t padded_size = (total_size + FLASH_WRITE_SIZE - 1) / FLASH_WRITE_SIZE * FLASH_WRITE_SIZE;
    err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t) g_firmware_buffer, APP_FIRMWARE_START_ADDRESS, padded_size);
    if (FSP_SUCCESS != err)
    {
        R_FLASH_HP_Close(&g_flash0_ctrl);
        return err;
    }

    R_FLASH_HP_Close(&g_flash0_ctrl);

    return FSP_SUCCESS;
}


/***********************************************************************************************************************
 * CAN Bootloader Implementation
 **********************************************************************************************************************/
void can_callback(can_callback_args_t *p_args)
{
    if (p_args->event == CAN_EVENT_RX_COMPLETE)
    {
        switch (g_receiver_state)
        {
            case STATE_IDLE:
            {
                if ((p_args->frame.id == CAN_ID_FILE_CONTROL) && (p_args->frame.data[0] == SOT_PACKET))
                {
                    memcpy(&g_expected_file_size, &p_args->frame.data[1], sizeof(g_expected_file_size));
                    SEGGER_RTT_printf(0, "[CAN] SOT Received. Expected size: %lu bytes\r\n", g_expected_file_size);

                    if (g_expected_file_size > FIRMWARE_BUFFER_SIZE)
                    {
                        SEGGER_RTT_printf(0, "[CAN] Error: File too large!\r\n");
                        g_receiver_state = STATE_ERROR;
                    }
                    else
                    {
                        g_received_size = 0;
                        g_can_next_sequence_no = 0;
                        memset(g_firmware_buffer, 0xFF, FIRMWARE_BUFFER_SIZE);
                        g_receiver_state = STATE_RECEIVING;
                    }
                }
                break;
            }

            case STATE_RECEIVING:
            {
                if (p_args->frame.id == CAN_ID_FILE_DATA)
                {
                    uint16_t received_seq_no;
                    memcpy(&received_seq_no, &p_args->frame.data[0], sizeof(received_seq_no));

                    if (received_seq_no == g_can_next_sequence_no)
                    {
                        uint8_t data_len = p_args->frame.data_length_code - 2;
                        uint32_t offset = g_can_next_sequence_no * CAN_DATA_MAX_SIZE;
                        memcpy(&g_firmware_buffer[offset], &p_args->frame.data[2], data_len);
                        g_received_size += data_len;
                        g_can_next_sequence_no++;
                    }
                }
                else if ((p_args->frame.id == CAN_ID_FILE_CONTROL) && (p_args->frame.data[0] == EOT_PACKET))
                {
                    if (g_received_size == g_expected_file_size)
                    {
                        SEGGER_RTT_printf(0, "[CAN] Transfer successful. %lu bytes received.\r\n", g_received_size);
                        SEGGER_RTT_printf(0, "[CAN] Now flashing firmware from RAM...\r\n");

                        fsp_err_t flash_err = flash_firmware_from_ram(g_received_size);

                        if(FSP_SUCCESS == flash_err)
                        {
                            SEGGER_RTT_printf(0, "[CAN] Flashing complete. Requesting Jump.\r\n");
                            g_transfer_complete = true;
                        }
                        else
                        {
                            SEGGER_RTT_printf(0, "[CAN] ** Flashing FAILED! Error: %d **\r\n", flash_err);
                            g_receiver_state = STATE_ERROR;
                        }
                    }
                    else
                    {
                        g_receiver_state = STATE_ERROR;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

/***********************************************************************************************************************
 * UART HEX Bootloader Implementation
 **********************************************************************************************************************/

static fsp_err_t hex_to_nibble(uint8_t hex_char, uint8_t * p_nibble)
{
    if ((hex_char >= '0') && (hex_char <= '9')) *p_nibble = (uint8_t)(hex_char - '0');
    else if ((hex_char >= 'A') && (hex_char <= 'F')) *p_nibble = (uint8_t)(hex_char - 'A' + 10);
    else if ((hex_char >= 'a') && (hex_char <= 'f')) *p_nibble = (uint8_t)(hex_char - 'a' + 10);
    else return FSP_ERR_INVALID_ARGUMENT;
    return FSP_SUCCESS;
}

static fsp_err_t hex_to_byte(uint8_t * p_hex_str, uint8_t * p_byte)
{
    uint8_t upper_nibble, lower_nibble;
    if (FSP_SUCCESS != hex_to_nibble(p_hex_str[0], &upper_nibble)) return FSP_ERR_INVALID_ARGUMENT;
    if (FSP_SUCCESS != hex_to_nibble(p_hex_str[1], &lower_nibble)) return FSP_ERR_INVALID_ARGUMENT;
    *p_byte = (uint8_t)((upper_nibble << 4) | lower_nibble);
    return FSP_SUCCESS;
}

static void parse_hex_line(char * p_line)
{
    uint8_t byte_count, record_type, checksum = 0, temp_byte;
    uint16_t address;

    if (p_line[0] != ':') return;

    hex_to_byte((uint8_t *)&p_line[1], &byte_count);
    checksum += byte_count;
    hex_to_byte((uint8_t *)&p_line[3], &temp_byte);
    address = (uint16_t)(temp_byte << 8);
    checksum += temp_byte;
    hex_to_byte((uint8_t *)&p_line[5], &temp_byte);
    address |= temp_byte;
    checksum += temp_byte;
    hex_to_byte((uint8_t *)&p_line[7], &record_type);
    checksum += record_type;

    SEGGER_RTT_printf(0, "[HEX DBG] RT: %d, Addr: 0x%04X, ", record_type, address);

    for (uint32_t i = 0; i < byte_count; i++)
    {
        hex_to_byte((uint8_t *)&p_line[9 + (i * 2)], &temp_byte);
        checksum += temp_byte;
    }
    hex_to_byte((uint8_t *)&p_line[9 + (byte_count * 2)], &temp_byte);
    checksum += temp_byte;

    if (checksum != 0)
    {
        SEGGER_RTT_printf(0, "CHECKSUM ERR!\r\n");
        g_receiver_state = STATE_ERROR;
        return;
    }

    uint32_t line_address = g_hex_base_address + address;
    SEGGER_RTT_printf(0, "FullAddr: 0x%08lX\r\n", line_address);

    /* Ignore Option Setting Memory (0x0100xxxx) or other non-Flash/RAM regions to prevent errors */
    if ((line_address >= 0x01000000) && (line_address < 0x02000000))
    {
        SEGGER_RTT_printf(0, "[HEX] Ignoring Option Byte Record at 0x%08X\r\n", line_address);
        return;
    }

    switch (record_type)
    {
        case 0x00:
        {
            uint32_t buffer_start_address = APP_FIRMWARE_START_ADDRESS;
            if ((line_address >= buffer_start_address) && ((line_address + byte_count) <= (buffer_start_address + FIRMWARE_BUFFER_SIZE)))
            {
                for (uint32_t i = 0; i < byte_count; i++)
                {
                    hex_to_byte((uint8_t *)&p_line[9 + (i * 2)], &g_firmware_buffer[line_address - buffer_start_address + i]);
                }
                g_received_size += byte_count;
            }
            else
            {
                SEGGER_RTT_printf(0, "[HEX DBG] Address out of range!\r\n");
            }
            break;
        }
        case 0x01:
        {
            SEGGER_RTT_printf(0, "[HEX] Transfer successful. %lu bytes reconstructed.\r\n", g_received_size);
            SEGGER_RTT_printf(0, "[HEX] Now flashing firmware from RAM...\r\n");

            fsp_err_t flash_err = flash_firmware_from_ram(g_received_size);

            if(FSP_SUCCESS == flash_err)
            {
                SEGGER_RTT_printf(0, "[HEX] Flashing complete. Resetting.\r\n");
                bootloader_jump_to_app();
            }
            else
            {
                SEGGER_RTT_printf(0, "[HEX] ** Flashing FAILED! Error: %d **\r\n", flash_err);
                g_receiver_state = STATE_ERROR;
            }
            break;
        }
        case 0x02:
        {
            uint16_t upper_address;
            hex_to_byte((uint8_t *)&p_line[9], &temp_byte);
            upper_address = (uint16_t)(temp_byte << 8);
            hex_to_byte((uint8_t *)&p_line[11], &temp_byte);
            upper_address |= temp_byte;
            g_hex_base_address = (uint32_t)(upper_address << 4); // Shift by 4 for segment address
            break;
        }
        case 0x03:
        case 0x05:
        {
            break;
        }
        case 0x04:
        {
            uint16_t upper_address;
            hex_to_byte((uint8_t *)&p_line[9], &temp_byte);
            upper_address = (uint16_t)(temp_byte << 8);
            hex_to_byte((uint8_t *)&p_line[11], &temp_byte);
            upper_address |= temp_byte;
            g_hex_base_address = (uint32_t)(upper_address << 16); // Shift by 16 for linear address
            break;
        }
        default:
            break;
    }
}

void uart_callback(uart_callback_args_t *p_args)
{
    if (UART_EVENT_RX_CHAR == p_args->event)
    {
        uint8_t byte = (uint8_t)p_args->data;
        if ((byte == '\n') || (byte == '\r'))
        {
            if (g_uart_line_idx > 1)
            {
                g_uart_line_buffer[g_uart_line_idx] = '\0';
                parse_hex_line((char *)g_uart_line_buffer);
                g_uart_line_idx = 0;
            }
        }
        else if (g_uart_line_idx < (UART_RX_LINE_BUFFER_SIZE - 1))
        {
            g_uart_line_buffer[g_uart_line_idx++] = byte;
        }
    }
}

void bootloader_uart_mode(void)
{
    g_receiver_state = STATE_RECEIVING;
    g_received_size = 0;
    g_hex_base_address = 0;
    memset(g_firmware_buffer, 0xFF, FIRMWARE_BUFFER_SIZE);

    fsp_err_t err = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    if (FSP_SUCCESS != err)
    {
        SEGGER_RTT_printf(0, "[HEX] R_SCI_UART_Open failed: %d\r\n", err);
        return;
    }
    while(g_receiver_state == STATE_RECEIVING)
    {
        /* All work is done in the UART callback */
    }
    R_SCI_UART_Close(&g_uart0_ctrl);
}


void bootloader_jump_to_app(void)
{
    /* Disable all maskable interrupts */
    __disable_irq();

    /* Disable SysTick to prevent it from firing in the App */
    SysTick->CTRL = 0;

    /* Clear MSPLIM to prevent stack overflow detection in App before it's set */
    __set_MSPLIM(0);

    /* Set the Vector Table Offset Register to the application's vector table. */
    SCB->VTOR = APP_FIRMWARE_START_ADDRESS;

    /* Disable and Clear all interrupts in the NVIC */
    for (IRQn_Type i = 0; i < 96; i++)
    {
        NVIC_DisableIRQ(i);
        NVIC_ClearPendingIRQ(i);
    }

    /* Set the main stack pointer from the application's vector table */
    __set_MSP(*(uint32_t *)APP_FIRMWARE_START_ADDRESS);

    /* Get the application's reset handler address from the vector table */
    void (*app_reset_handler)(void) = (void (*)(void))(*(uint32_t *)(APP_FIRMWARE_START_ADDRESS + 4));

    /* Jump to the application's reset handler */
    app_reset_handler();
}

/* Redundant callback, keep for FSP compatibility */
void flash_callback(flash_callback_args_t *p_args)
{
    (void)p_args;
}
