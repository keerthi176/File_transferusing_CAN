/***********************************************************************************************************************
 * File Name    : ymodem.h
 * Description  : YMODEM protocol implementation header.
 ***********************************************************************************************************************/

#ifndef YMODEM_H_
#define YMODEM_H_

#include <stdint.h>
#include <stdbool.h>

/* YMODEM protocol control characters */
#define SOH                     (0x01)  /* Start of Header (128-byte data packet) */
#define STX                     (0x02)  /* Start of Text (1024-byte data packet) */
#define EOT                     (0x04)  /* End of Transmission */
#define ACK                     (0x06)  /* Acknowledge */
#define NAK                     (0x15)  /* Negative Acknowledge */
#define CAN                     (0x18)  /* Cancel */
#define CRC_CHAR                ('C')   /* Character to request CRC-16 mode */

#define YMODEM_PACKET_HEADER_SIZE       (3)
#define YMODEM_PACKET_TRAILER_SIZE      (2)     /* CRC-16 */
#define YMODEM_PACKET_OVERHEAD_SIZE     (YMODEM_PACKET_HEADER_SIZE + YMODEM_PACKET_TRAILER_SIZE)
#define YMODEM_SOH_PACKET_SIZE          (128)
#define YMODEM_STX_PACKET_SIZE          (1024)

/**
 * @brief Structure of callback functions for YMODEM protocol.
 */
typedef struct
{
    int (*_getc)(uint8_t *c, uint32_t timeout);
    void (*_putc)(uint8_t c);
    int (*on_start)(char *filename, uint32_t filesize);
    int (*on_data)(uint8_t *data, uint32_t len);
    void (*on_finish)(void);
} ymodem_callbacks_t;


/* Public function prototypes */
int ymodem_receive(ymodem_callbacks_t *p_callbacks);

#endif /* YMODEM_H_ */

