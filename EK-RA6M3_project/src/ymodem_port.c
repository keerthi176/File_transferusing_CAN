/***********************************************************************************************************************
 * File Name    : ymodem_port.c
 * Description  : Porting layer for the YMODEM protocol. Handles hardware-specific I/O and FileX operations.
 **********************************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "hal_data.h"
#include "r_sci_uart.h"
#include "fx_api.h"
#include "filex_block_media_qspi_levelx_ep.h"

#include "ymodem.h"

#include "common_utils.h"

extern const uart_instance_t g_uart0;
extern FX_MEDIA g_fx_media0;
extern uint8_t g_fx_media0_media_memory[G_FX_MEDIA0_MEDIA_MEMORY_SIZE];


/* FileX variables for this transfer */
static FX_FILE g_ymodem_file;
static bool g_file_is_open = false;

/* Buffer to store the received filename */
static char * g_p_filename_buffer = NULL;
static uint32_t g_filename_buffer_size = 0;


/* Volatile flags for UART events */
static volatile bool g_uart_tx_complete = false;
static volatile bool g_uart_rx_ready = false;

/* Static function prototypes */
static int _ymodem_read_byte_wrapper(uint8_t *p_byte, uint32_t timeout_ms);
static void _ymodem_send_byte_wrapper(uint8_t byte);
static int _ymodem_on_start(char *filename, uint32_t filesize);
static int _ymodem_on_data(uint8_t *data, uint32_t len);
static void _ymodem_on_finish(void);
void ymodem_uart_callback (uart_callback_args_t * p_args);


/*******************************************************************************************************************//**
 * @brief       Main entry point for starting the YMODEM receive process.
 * @param[IN]   filename_buffer         Pointer to a buffer to store the received filename.
 * @param[IN]   buffer_size             Size of the filename buffer.
 * @retval      0 on success, -1 on error.
 **********************************************************************************************************************/
int ymodem_start_receive(char *filename_buffer, uint32_t buffer_size)
{
    fsp_err_t err = FSP_SUCCESS;
    int result = 0;

    /* Close the port forcefully to ensure it's in a known-good state.
     * This is a defensive measure against state issues from previous failed transfers. */
    R_SCI_UART_Close(g_uart0.p_ctrl);

    /* Store the application's filename buffer */
    g_p_filename_buffer = filename_buffer;
    g_filename_buffer_size = buffer_size;
    if(g_p_filename_buffer)
    {
        g_p_filename_buffer[0] = '\0';
    }

    /* Create a non-const config struct to set the callback */
    uart_cfg_t ymodem_uart_cfg = *g_uart0.p_cfg;
    ymodem_uart_cfg.p_callback = ymodem_uart_callback;

    /* Create a mutable copy of the extended config to override flow control */
    sci_uart_extended_cfg_t ymodem_uart_extended_cfg = *(sci_uart_extended_cfg_t*)g_uart0.p_cfg->p_extend;

    /* Override flow control setting to effectively disable it */
    ymodem_uart_extended_cfg.flow_control = (sci_uart_flow_control_t) 0; // Set to a known value, RTS=0
    ymodem_uart_extended_cfg.flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX; // Ensure no pin is assigned

    /* Point the main config to our modified extended config */
    ymodem_uart_cfg.p_extend = &ymodem_uart_extended_cfg;


    /* Open UART for the transfer */
    err = R_SCI_UART_Open(g_uart0.p_ctrl, &ymodem_uart_cfg);
    if(FSP_SUCCESS != err && FSP_ERR_ALREADY_OPEN != err)
    {
        PRINT_ERR_STR("Failed to open UART for YMODEM\r\n");
        return -1;
    }

    /* Populate the callbacks structure */
    ymodem_callbacks_t ymodem_calls =
    {
      ._getc = _ymodem_read_byte_wrapper,
      ._putc = _ymodem_send_byte_wrapper,
      .on_start = _ymodem_on_start,
      .on_data = _ymodem_on_data,
      .on_finish = _ymodem_on_finish,
    };

    /* Call the generic YMODEM receive function */
    result = ymodem_receive(&ymodem_calls);

    /* Close the UART */
    R_SCI_UART_Close(g_uart0.p_ctrl);

    return result;
}

static int _ymodem_on_start(char *filename, uint32_t filesize)
{
    UINT fx_status;
    (void)filesize;

    /* Store the filename in the application's buffer if provided */
    if(g_p_filename_buffer)
    {
        strncpy(g_p_filename_buffer, filename, g_filename_buffer_size);
    }

    /* Check if media is already open. If not, open it. */
    if (g_fx_media0.fx_media_id != FX_MEDIA_ID)
    {
        fx_status = fx_media_open(&g_fx_media0, (CHAR *)"&g_fx_media0", RM_FILEX_LEVELX_NOR_DeviceDriver,
                               (void *) &g_rm_filex_levelx_nor_instance, g_fx_media0_media_memory, G_FX_MEDIA0_MEDIA_MEMORY_SIZE);
        if(FX_SUCCESS != fx_status)
        {
            PRINT_ERR_STR("YMODEM: Media open failed.\r\n");
            return -1;
        }
    }

    fx_status = fx_file_create(&g_fx_media0, filename);
    if(FX_SUCCESS != fx_status && FX_ALREADY_CREATED != fx_status)
    {
        PRINT_ERR_STR("YMODEM: File create failed.\r\n");
        fx_media_close(&g_fx_media0);
        return -1;
    }

    fx_status = fx_file_open(&g_fx_media0, &g_ymodem_file, filename, FX_OPEN_FOR_WRITE);
    if(FX_SUCCESS != fx_status)
    {
        PRINT_ERR_STR("YMODEM: File open for write failed.\r\n");
        fx_media_close(&g_fx_media0);
        return -1;
    }

    g_file_is_open = true;
    return 0;
}

static int _ymodem_on_data(uint8_t *data, uint32_t len)
{
    if(!g_file_is_open) return -1;

    if (FX_SUCCESS != fx_file_write(&g_ymodem_file, data, len))
    {
        PRINT_ERR_STR("YMODEM: FileX write failed.\r\n");
        return -1;
    }
    return 0;
}

static void _ymodem_on_finish(void)
{
    if(g_file_is_open)
    {
        fx_file_close(&g_ymodem_file);
        fx_media_close(&g_fx_media0);
        g_file_is_open = false;
    }
}

static int _ymodem_read_byte_wrapper(uint8_t *p_byte, uint32_t timeout_ms)
{
    if (!p_byte) return -1;

    g_uart_rx_ready = false;
    fsp_err_t err = R_SCI_UART_Read(g_uart0.p_ctrl, p_byte, 1);
    if(FSP_SUCCESS != err)
    {
        APP_PRINT("DBG: R_SCI_UART_Read failed with error: %d\r\n", err);
        return -1;
    }

    /* Wait for the read to complete or timeout */
    for(uint32_t i = 0; i < timeout_ms; i++)
    {
        if(g_uart_rx_ready)
        {
            APP_PRINT("DBG: UART RX Byte: 0x%02X\r\n", *p_byte);
            return 0; // Success
        }
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }

    APP_PRINT("DBG: UART Read Byte Timeout\r\n");
    R_SCI_UART_Abort(g_uart0.p_ctrl, UART_DIR_RX);
    return -1;
}

static void _ymodem_send_byte_wrapper(uint8_t byte)
{
    g_uart_tx_complete = false;
    APP_PRINT("DBG: UART TX: 0x%02X\r\n", byte);
    R_SCI_UART_Write(g_uart0.p_ctrl, &byte, 1);
    while(!g_uart_tx_complete)
    {
        /* Wait for TX complete flag set by the callback */
         R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }
}

void ymodem_uart_callback (uart_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_TX_COMPLETE:
            g_uart_tx_complete = true;
            break;
        case UART_EVENT_RX_COMPLETE:
            g_uart_rx_ready = true;
            break;
        case UART_EVENT_ERR_PARITY:
            APP_PRINT("YMODEM ERR: UART Parity Error\r\n");
            break;
        case UART_EVENT_ERR_FRAMING:
            APP_PRINT("YMODEM ERR: UART Framing Error\r\n");
            break;
        case UART_EVENT_ERR_OVERFLOW:
            APP_PRINT("YMODEM ERR: UART Overflow Error\r\n");
            break;
        default:
            break;
    }
}

/*******************************************************************************************************************//**
 * @brief       Simple UART echo test function for hardware diagnostics.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void uart_echo_test(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t received_char;

    /* Create a non-const config struct to set the callback */
    uart_cfg_t echo_uart_cfg = *g_uart0.p_cfg;
    echo_uart_cfg.p_callback = ymodem_uart_callback;

    /* Open UART for the test */
    err = R_SCI_UART_Open(g_uart0.p_ctrl, &echo_uart_cfg);
    if(FSP_SUCCESS != err && FSP_ERR_ALREADY_OPEN != err)
    {
        PRINT_ERR_STR("Failed to open UART for Echo Test\r\n");
        return;
    }

    PRINT_INFO_STR("Starting UART Echo Test. Type in your terminal. Press ESC to exit.\r\n");

    while(1)
    {
        g_uart_rx_ready = false;
        err = R_SCI_UART_Read(g_uart0.p_ctrl, &received_char, 1);
        if (err != FSP_SUCCESS)
        {
            APP_PRINT("Echo: Read failed, restarting read.\r\n");
            continue;
        }

        /* Wait for a character */
        while(!g_uart_rx_ready)
        {
            tx_thread_sleep(10); // Sleep for 10 ticks
        }

        /* Print the received character to RTT */
        APP_PRINT("ECHO RX: 0x%02X\r\n", received_char);

        /* Check for ESC key to exit */
        if (0x1B == received_char)
        {
            PRINT_INFO_STR("Exiting UART Echo Test.\r\n");
            break;
        }

        /* Echo the character back */
        g_uart_tx_complete = false;
        err = R_SCI_UART_Write(g_uart0.p_ctrl, &received_char, 1);
        if (FSP_SUCCESS == err)
        {
            while(!g_uart_tx_complete)
            {
                tx_thread_sleep(1);
            }
        }
    }

    /* Close the UART */
    R_SCI_UART_Close(g_uart0.p_ctrl);
}