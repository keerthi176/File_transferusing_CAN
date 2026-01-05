#include "bootloader.h"
#include "common_utils.h"
#include "bsp_api.h"

#define CODE_FLASH_BLOCK_SIZE (0x2000)
#define FLASH_WRITE_SIZE      (128)

static receiver_state_t g_receiver_state = STATE_IDLE;
uint8_t g_firmware_buffer[FIRMWARE_BUFFER_SIZE];
static uint32_t g_received_size = 0;
static uint32_t g_expected_file_size = 0;
static uint16_t g_next_sequence_no = 0;

void bootloader_init(void)
{
    SEGGER_RTT_Init();
    SEGGER_RTT_printf(0, "[BOOTLOADER] Starting...\r\n");
    /* Initialize the CAN driver */
    fsp_err_t err = R_CAN_Open(&g_can0_ctrl, &g_can0_cfg);
    if (FSP_SUCCESS != err)
    {
        SEGGER_RTT_printf(0, "[BOOTLOADER] R_CAN_Open failed: %d\r\n", err);
    }
    else
    {
        SEGGER_RTT_printf(0, "[BOOTLOADER] CAN driver opened. Waiting for firmware...\r\n");
    }
}

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
                    if (p_args->frame.data_length_code == 5)
                    {
                        memcpy(&g_expected_file_size, &p_args->frame.data[1], sizeof(g_expected_file_size));
                        SEGGER_RTT_printf(0, "[BOOTLOADER] SOT received. Expecting %lu bytes.\r\n", g_expected_file_size);
                        if (g_expected_file_size > FIRMWARE_BUFFER_SIZE)
                        {
                            SEGGER_RTT_printf(0, "[BOOTLOADER] ERROR: Firmware size (%lu) exceeds buffer size (%d).\r\n", g_expected_file_size, FIRMWARE_BUFFER_SIZE);
                            g_receiver_state = STATE_ERROR;
                        }
                        else
                        {
                            g_received_size = 0;
                            g_next_sequence_no = 0;
                            memset(g_firmware_buffer, 0xFF, FIRMWARE_BUFFER_SIZE); // Fill with 0xFF
                            g_receiver_state = STATE_RECEIVING;
                            SEGGER_RTT_printf(0, "[BOOTLOADER] State -> STATE_RECEIVING.\r\n");
                        }
                    }
                    else
                    {
                        SEGGER_RTT_printf(0, "[BOOTLOADER] ERROR: Invalid SOT DLC: %d.\r\n", p_args->frame.data_length_code);
                        g_receiver_state = STATE_ERROR;
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

                    if (received_seq_no == g_next_sequence_no)
                    {
                        uint8_t data_len = p_args->frame.data_length_code - 2;
                        uint32_t offset = g_next_sequence_no * CAN_DATA_MAX_SIZE;

                        if ((offset + data_len) <= g_expected_file_size)
                        {
                            memcpy(&g_firmware_buffer[offset], &p_args->frame.data[2], data_len);
                            g_received_size += data_len;
                            g_next_sequence_no++;
                        }
                        else
                        {
                            SEGGER_RTT_printf(0, "[BOOTLOADER] ERROR: Received data would overflow buffer.\r\n");
                            g_receiver_state = STATE_ERROR;
                        }
                    }
                    else
                    {
                        SEGGER_RTT_printf(0, "[BOOTLOADER] ERROR: Out of order packet. Expected %u, got %u.\r\n", g_next_sequence_no, received_seq_no);
                        g_receiver_state = STATE_ERROR;
                    }
                }
                else if ((p_args->frame.id == CAN_ID_FILE_CONTROL) && (p_args->frame.data[0] == EOT_PACKET))
                {
                    SEGGER_RTT_printf(0, "[BOOTLOADER] EOT received.\r\n");
                    if (g_received_size == g_expected_file_size)
                    {
                        SEGGER_RTT_printf(0, "[BOOTLOADER] Transfer successful. Reconstructed %lu bytes.\r\n", g_received_size);
                        g_receiver_state = STATE_COMPLETE;
                        SEGGER_RTT_printf(0, "[BOOTLOADER] State -> STATE_COMPLETE.\r\n");

                        uint32_t padded_size = (g_expected_file_size + FLASH_WRITE_SIZE - 1) / FLASH_WRITE_SIZE * FLASH_WRITE_SIZE;
                        SEGGER_RTT_printf(0, "[BOOTLOADER] Original size: %lu, Padded size for flash: %lu.\r\n", g_expected_file_size, padded_size);

                        /* Open flash driver for writing */
                        fsp_err_t err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
                        if (FSP_SUCCESS != err)
                        {
                            SEGGER_RTT_printf(0, "[BOOTLOADER] R_FLASH_HP_Open failed: %d\r\n", err);
                            g_receiver_state = STATE_ERROR;
                            break;
                        }

                        SEGGER_RTT_printf(0, "[BOOTLOADER] Starting flash erase...\r\n");
                        err = R_FLASH_HP_Erase(&g_flash0_ctrl, APP_FIRMWARE_START_ADDRESS, (padded_size + CODE_FLASH_BLOCK_SIZE - 1) / CODE_FLASH_BLOCK_SIZE);
                        if(err != FSP_SUCCESS)
                        {
                            SEGGER_RTT_printf(0, "[BOOTLOADER] Flash Erase Failed: %d\r\n", err);
                            g_receiver_state = STATE_ERROR;
                            R_FLASH_HP_Close(&g_flash0_ctrl); // Close on error
                            break;
                        }
                        SEGGER_RTT_printf(0, "[BOOTLOADER] Flash erase complete.\r\n");


                        SEGGER_RTT_printf(0, "[BOOTLOADER] Starting flash write...\r\n");
                        err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)g_firmware_buffer, APP_FIRMWARE_START_ADDRESS, padded_size);
                        if(err != FSP_SUCCESS)
                        {
                            SEGGER_RTT_printf(0, "[BOOTLOADER] Flash Write Failed: %d\r\n", err);
                            g_receiver_state = STATE_ERROR;
                            R_FLASH_HP_Close(&g_flash0_ctrl); // Close on error
                            break;
                        }
                        SEGGER_RTT_printf(0, "[BOOTLOADER] Flash write complete.\r\n");

                        /* Close the flash driver */
                        R_FLASH_HP_Close(&g_flash0_ctrl);

                        SEGGER_RTT_printf(0, "[BOOTLOADER] Flash complete. Setting marker and resetting.\r\n");
                        bootloader_jump_to_app();
                    }
                    else
                    {
                        SEGGER_RTT_printf(0, "[BOOTLOADER] ERROR: File size mismatch. Expected %lu, got %lu.\r\n", g_expected_file_size, g_received_size);
                        g_receiver_state = STATE_ERROR;
                    }
                }
                break;
            }

            case STATE_COMPLETE:
            case STATE_ERROR:
            {
                if(g_receiver_state == STATE_ERROR)
                {
                     SEGGER_RTT_printf(0, "[BOOTLOADER] In STATE_ERROR. Waiting for new SOT.\r\n");
                }
                // In an error state, we can wait for a new SOT or a reset
                if ((p_args->frame.id == CAN_ID_FILE_CONTROL) && (p_args->frame.data[0] == SOT_PACKET))
                {
                    g_receiver_state = STATE_IDLE;
                    // Re-process the SOT packet
                     if (p_args->frame.data_length_code == 5)
                    {
                        memcpy(&g_expected_file_size, &p_args->frame.data[1], sizeof(g_expected_file_size));
                        SEGGER_RTT_printf(0, "[BOOTLOADER] SOT received. Expecting %lu bytes.\r\n", g_expected_file_size);
                        if (g_expected_file_size > FIRMWARE_BUFFER_SIZE)
                        {
                            SEGGER_RTT_printf(0, "[BOOTLOADER] ERROR: Firmware size (%lu) exceeds buffer size (%d).\r\n", g_expected_file_size, FIRMWARE_BUFFER_SIZE);
                            g_receiver_state = STATE_ERROR;
                        }
                        else
                        {
                            g_received_size = 0;
                            g_next_sequence_no = 0;
                            memset(g_firmware_buffer, 0xFF, FIRMWARE_BUFFER_SIZE); // Fill with 0xFF
                            g_receiver_state = STATE_RECEIVING;
                            SEGGER_RTT_printf(0, "[BOOTLOADER] State -> STATE_RECEIVING.\r\n");
                        }
                    }
                    else
                    {
                        SEGGER_RTT_printf(0, "[BOOTLOADER] ERROR: Invalid SOT DLC: %d.\r\n", p_args->frame.data_length_code);
                        g_receiver_state = STATE_ERROR;
                    }
                }
                break;
            }

            default:
            {
                g_receiver_state = STATE_IDLE;
                break;
            }
        }
    }
     else if (p_args->event != CAN_EVENT_TX_COMPLETE)
    {
        SEGGER_RTT_printf(0, "[BOOTLOADER] CAN Error Event: %lu\r\n", p_args->event);
    }
}
void bootloader_jump_to_app(void)
{
    /* Unlock system registers */
    R_SYSTEM->PRCR = (uint16_t) (0xA500 | 2);

    /* Write magic word to backup registers */
    R_SYSTEM->VBTBKR[0] = (uint8_t) (APP_VALID_MARKER >> 24);
    R_SYSTEM->VBTBKR[1] = (uint8_t) (APP_VALID_MARKER >> 16);
    R_SYSTEM->VBTBKR[2] = (uint8_t) (APP_VALID_MARKER >> 8);
    R_SYSTEM->VBTBKR[3] = (uint8_t) (APP_VALID_MARKER >> 0);

    /* Lock system registers */
    R_SYSTEM->PRCR = (uint16_t) (0xA500 | 0);

    /* De-initialize peripherals */
    R_CAN_Close(&g_can0_ctrl);
    R_FLASH_HP_Close(&g_flash0_ctrl);

    /* Reset the MCU */
    NVIC_SystemReset();
}

void flash_callback(flash_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case FLASH_EVENT_ERASE_COMPLETE:
            SEGGER_RTT_printf(0, "[CALLBACK] Event: Erase Complete.\r\n");
            break;
        case FLASH_EVENT_WRITE_COMPLETE:
            SEGGER_RTT_printf(0, "[CALLBACK] Event: Write Complete.\r\n");
            break;
        case FLASH_EVENT_BLANK:
            SEGGER_RTT_printf(0, "[CALLBACK] Event: Blank Check OK.\r\n");
            break;
        case FLASH_EVENT_NOT_BLANK:
            SEGGER_RTT_printf(0, "[CALLBACK] Event: Blank Check Fail (Not Blank).\r\n");
            break;
        case FLASH_EVENT_ERR_DF_ACCESS:
            SEGGER_RTT_printf(0, "[CALLBACK] ** ERROR: Data Flash Access. **\r\n");
            break;
        case FLASH_EVENT_ERR_CF_ACCESS:
            SEGGER_RTT_printf(0, "[CALLBACK] ** ERROR: Code Flash Access. **\r\n");
            break;
        case FLASH_EVENT_ERR_CMD_LOCKED:
            SEGGER_RTT_printf(0, "[CALLBACK] ** ERROR: Command Locked. **\r\n");
            break;
        case FLASH_EVENT_ERR_FAILURE:
            SEGGER_RTT_printf(0, "[CALLBACK] ** ERROR: Generic Failure. **\r\n");
            break;
        default:
            SEGGER_RTT_printf(0, "[CALLBACK] Unknown Event ID: %d\r\n", (int)p_args->event);
            break;
    }
}


/***********************************************************************************************************************
 * UART HEX Bootloader Implementation
 **********************************************************************************************************************/

#define UART_RX_LINE_BUFFER_SIZE (128)
static uint8_t g_uart_line_buffer[UART_RX_LINE_BUFFER_SIZE];
static uint32_t g_uart_line_idx = 0;

/* Helper function to convert a single hex character (0-9, A-F) to its nibble value */
static fsp_err_t hex_to_nibble(uint8_t hex_char, uint8_t * p_nibble)
{
    if ((hex_char >= '0') && (hex_char <= '9'))
    {
        *p_nibble = (uint8_t)(hex_char - '0');
    }
    else if ((hex_char >= 'A') && (hex_char <= 'F'))
    {
        *p_nibble = (uint8_t)(hex_char - 'A' + 10);
    }
    else if ((hex_char >= 'a') && (hex_char <= 'f'))
    {
        *p_nibble = (uint8_t)(hex_char - 'a' + 10);
    }
    else
    {
        return FSP_ERR_INVALID_ARGUMENT; // Not a valid hex character
    }
    return FSP_SUCCESS;
}

/* Helper function to convert a two-character hex string to a byte */
static fsp_err_t hex_to_byte(uint8_t * p_hex_str, uint8_t * p_byte)
{
    uint8_t upper_nibble;
    uint8_t lower_nibble;

    if (FSP_SUCCESS != hex_to_nibble(p_hex_str[0], &upper_nibble))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }
    if (FSP_SUCCESS != hex_to_nibble(p_hex_str[1], &lower_nibble))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    *p_byte = (uint8_t)((upper_nibble << 4) | lower_nibble);
    return FSP_SUCCESS;
}

#define FLASH_PAGE_BUFFER_SIZE (128) // Minimum programming size for RA4M2 code flash is 128 bytes
static uint8_t g_flash_page_buffer[FLASH_PAGE_BUFFER_SIZE];
static uint32_t g_page_buffer_address = 0;
static uint32_t g_page_buffer_idx = 0;
static uint32_t g_hex_base_address = 0;
static bool g_app_erased = false;

static fsp_err_t flash_page_buffer(void)
{
    fsp_err_t err = FSP_SUCCESS;
    if (g_page_buffer_idx > 0)
    {
        /* Pad buffer with 0xFF to be a full page size if it's not full */
        uint32_t remainder = g_page_buffer_idx % FLASH_PAGE_BUFFER_SIZE;
        if(remainder != 0)
        {
            memset(&g_flash_page_buffer[g_page_buffer_idx], 0xFF, FLASH_PAGE_BUFFER_SIZE - remainder);
        }

        err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)g_flash_page_buffer, g_page_buffer_address, FLASH_PAGE_BUFFER_SIZE);
        if (FSP_SUCCESS != err)
        {
            SEGGER_RTT_printf(0, "[HEX] R_FLASH_HP_Write failed: %d\r\n", err);
            return err;
        }
    }

    /* Reset buffer */
    g_page_buffer_idx = 0;
    g_page_buffer_address = 0;
    memset(g_flash_page_buffer, 0xFF, FLASH_PAGE_BUFFER_SIZE);

    return FSP_SUCCESS;
}

static void parse_hex_line(char * p_line)
{
    fsp_err_t err;
    uint8_t byte_count;
    uint16_t address;
    uint8_t record_type;
    uint8_t checksum = 0;
    uint8_t temp_byte;

    if (p_line[0] != ':')
    {
        return; /* Not a valid HEX record */
    }

    /* Byte Count */
    hex_to_byte((uint8_t *)&p_line[1], &byte_count);
    checksum += byte_count;

    /* Address */
    hex_to_byte((uint8_t *)&p_line[3], &temp_byte);
    address = (uint16_t)(temp_byte << 8);
    checksum += temp_byte;
    hex_to_byte((uint8_t *)&p_line[5], &temp_byte);
    address |= temp_byte;
    checksum += temp_byte;

    /* Record Type */
    hex_to_byte((uint8_t *)&p_line[7], &record_type);
    checksum += record_type;

    /* Data Bytes */
    for (uint32_t i = 0; i < byte_count; i++)
    {
        hex_to_byte((uint8_t *)&p_line[9 + (i * 2)], &temp_byte);
        checksum += temp_byte;
    }

    /* Checksum */
    hex_to_byte((uint8_t *)&p_line[9 + (byte_count * 2)], &temp_byte);
    checksum += temp_byte;

    if (checksum != 0)
    {
        SEGGER_RTT_printf(0, "[HEX] Checksum error!\r\n");
        return;
    }

    /* Process Record */
    switch (record_type)
    {
        case 0x00: // Data Record
        {
            if (!g_app_erased)
            {
                /* This is the first data record. Open flash, and erase the entire application area. */
                err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
                if (FSP_SUCCESS != err)
                {
                    SEGGER_RTT_printf(0, "[HEX] R_FLASH_HP_Open failed: %d\r\n", err);
                    return; // Abort
                }

                uint32_t app_size = 0x70000; // 512KB - 64KB
                uint32_t blocks_to_erase = (app_size + (CODE_FLASH_BLOCK_SIZE - 1)) / CODE_FLASH_BLOCK_SIZE;
                SEGGER_RTT_printf(0, "[HEX] First data record. Erasing %lu blocks of app area...\r\n", blocks_to_erase);

                err = R_FLASH_HP_Erase(&g_flash0_ctrl, APP_FIRMWARE_START_ADDRESS, blocks_to_erase);
                if (FSP_SUCCESS != err)
                {
                    SEGGER_RTT_printf(0, "[HEX] Flash erase command failed: %d\r\n", err);
                    return;
                }
                SEGGER_RTT_printf(0, "[HEX] Erase complete.\r\n");
                g_app_erased = true;
            }

            uint32_t line_address = g_hex_base_address + address;

            for (uint32_t i = 0; i < byte_count; i++)
            {
                hex_to_byte((uint8_t *)&p_line[9 + (i * 2)], &temp_byte);

                /* If the buffer is empty, set its starting address */
                if (g_page_buffer_idx == 0)
                {
                    g_page_buffer_address = line_address + i;
                }

                /* If data is not contiguous or buffer is full, flash the existing buffer first */
                if (((line_address + i) != (g_page_buffer_address + g_page_buffer_idx)) || (g_page_buffer_idx >= FLASH_PAGE_BUFFER_SIZE))
                {
                    flash_page_buffer();
                    g_page_buffer_address = line_address + i; // Start new page
                }

                g_flash_page_buffer[g_page_buffer_idx++] = temp_byte;
            }
            break;
        }
        case 0x01: // End of File
        {
            /* Flash any remaining data in the buffer */
            flash_page_buffer();
            SEGGER_RTT_printf(0, "[HEX] End of File record received. Flashing complete.\r\n");

            /* Close the flash driver */
            R_FLASH_HP_Close(&g_flash0_ctrl);

            SEGGER_RTT_printf(0, "Setting marker and resetting...\r\n");
            bootloader_jump_to_app();
            break;
        }
        case 0x02: // Extended Segment Address Record
        case 0x03: // Start Segment Address Record
        case 0x05: // Start Linear Address Record
        {
            // Ignore these record types for this simple bootloader
            break;
        }
        case 0x04: // Extended Linear Address Record
        {
            uint16_t upper_address;
            hex_to_byte((uint8_t *)&p_line[9], &temp_byte);
            upper_address = (uint16_t)(temp_byte << 8);
            hex_to_byte((uint8_t *)&p_line[11], &temp_byte);
            upper_address |= temp_byte;
            g_hex_base_address = (uint32_t)(upper_address << 16);
            break;
        }
        default:
        {
            SEGGER_RTT_printf(0, "[HEX] Unknown record type: %02X\r\n", record_type);
            break;
        }
    }
}

void uart_callback(uart_callback_args_t *p_args)
{
    if (UART_EVENT_RX_CHAR == p_args->event)
    {
        uint8_t byte = (uint8_t)p_args->data;

        /* Check for end of line characters */
        if ((byte == '\n') || (byte == '\r'))
        {
            if (g_uart_line_idx > 1) // We have a line with some content
            {
                g_uart_line_buffer[g_uart_line_idx] = '\0'; // Null terminate the line
                parse_hex_line((char *)g_uart_line_buffer);

                /* Reset buffer for next line */
                g_uart_line_idx = 0;
                memset(g_uart_line_buffer, 0, UART_RX_LINE_BUFFER_SIZE);
            }
        }
        else if (g_uart_line_idx < (UART_RX_LINE_BUFFER_SIZE -1))
        {
            /* Add character to buffer */
            g_uart_line_buffer[g_uart_line_idx++] = byte;
        }
        else
        {
            /* Line buffer overflow, reset */
            g_uart_line_idx = 0;
        }
    }
}

void bootloader_uart_mode(void)
{
    fsp_err_t err = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    if (FSP_SUCCESS != err)
    {
        SEGGER_RTT_printf(0, "[HEX] R_SCI_UART_Open failed: %d\r\n", err);
        return;
    }

    /* Initialize flash page buffer */
    memset(g_flash_page_buffer, 0xFF, FLASH_PAGE_BUFFER_SIZE);

    /* All processing is now done in the uart_callback ISR. */
    /* This loop just keeps the program alive. */
    while(1)
    {
        /* Do nothing here. */
    }
}