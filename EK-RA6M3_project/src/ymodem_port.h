/***********************************************************************************************************************
 * File Name    : ymodem_port.h
 * Description  : Header for the YMODEM porting layer.
 ***********************************************************************************************************************/

#ifndef YMODEM_PORT_H_
#define YMODEM_PORT_H_

#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"          // For uart_instance_t
#include "r_sci_uart.h"        // For uart_callback_args_t, uart_cfg_t
#include "fx_api.h"            // For FX_MEDIA, FX_FILE
#include "ymodem.h"            // For ymodem_callbacks_t
#include "filex_block_media_qspi_levelx_ep.h" // For MAX_FILE_NAME_SIZE, G_FX_MEDIA0_MEDIA_MEMORY_SIZE

/* Public function prototypes */
int ymodem_start_receive(char *filename_buffer, uint32_t buffer_size);
void uart_echo_test(void);

#endif /* YMODEM_PORT_H_ */
