/***********************************************************************************************************************
 * File Name    : ymodem.c
 * Description  : Generic YMODEM protocol implementation.
 ***********************************************************************************************************************/

#include <string.h>
#include <stdlib.h> // Required for atoi
#include "ymodem.h"
#include "common_utils.h"

/* Static function prototypes */
static int32_t _receive_packet(uint8_t *p_data, int32_t *p_length, ymodem_callbacks_t *p_callbacks);
static uint16_t _crc16_update(uint16_t crc_in, uint8_t byte);
static uint16_t _crc16_calculate(const uint8_t *p_data, uint32_t size);

/*******************************************************************************************************************//**
 * @brief       Generic YMODEM receive function.
 * @param[IN]   p_callbacks   Pointer to a structure of callback functions for I/O and file operations.
 * @retval      0 on success, -1 on error.
 **********************************************************************************************************************/
int ymodem_receive(ymodem_callbacks_t *p_callbacks)
{
    uint8_t packet_data[YMODEM_STX_PACKET_SIZE + YMODEM_PACKET_OVERHEAD_SIZE];
    char file_name[128];
    char file_size_str[16];
    int32_t packet_length = 0;
    uint32_t file_size = 0;
    int result = 0;

    APP_PRINT("Starting YMODEM receive. Please initiate transfer from your terminal...\r\n");

    /* Send 'C' to start transfer with CRC */
    APP_PRINT("YMODEM: Sending initial 'C'\r\n");
    p_callbacks->_putc(CRC_CHAR);

    /* --- Receive Header Packet (Packet 0) --- */
    APP_PRINT("YMODEM: Waiting for header packet...\r\n");
    if (_receive_packet(packet_data, &packet_length, p_callbacks) != 0 || packet_length == 0)
    {
        APP_PRINT("YMODEM ERR: Failed to receive header packet.\r\n");
        return -1;
    }

    if (packet_data[0] != 0)
    {
        APP_PRINT("YMODEM ERR: Invalid header packet number.\r\n");
        p_callbacks->_putc(CAN);
        p_callbacks->_putc(CAN);
        return -1;
    }

    /* Null-terminated filename starts at data[1] */
    strncpy(file_name, (char const *)&packet_data[1], sizeof(file_name));
    strncpy(file_size_str, (char const *)&packet_data[2 + strlen(file_name)], sizeof(file_size_str));
    file_size = (uint32_t)atoi(file_size_str);

    APP_PRINT("YMODEM: Received File: %s, Size: %lu bytes\r\n", file_name, file_size);

    if (p_callbacks->on_start(file_name, file_size) != 0)
    {
        p_callbacks->_putc(CAN);
        p_callbacks->_putc(CAN);
        return -1;
    }

    /* --- Acknowledge Header and Start Data Transfer --- */
    APP_PRINT("YMODEM: Sending ACK and 'C' for data start\r\n");
    p_callbacks->_putc(ACK);
    p_callbacks->_putc(CRC_CHAR);

    /* --- Receive Data Packets --- */
    uint32_t bytes_received = 0;
    while(bytes_received < file_size)
    {
        APP_PRINT("YMODEM: Waiting for data packet...\r\n");
        result = _receive_packet(packet_data, &packet_length, p_callbacks);
        if (result != 0)
        {
            APP_PRINT("YMODEM ERR: Failed receiving data packet.\r\n");
            p_callbacks->on_finish();
            return -1;
        }

        if (packet_length > 0)
        {
             uint32_t bytes_to_write = (uint32_t)packet_length;
             if ((bytes_received + bytes_to_write) > file_size)
             {
                 bytes_to_write = file_size - bytes_received;
             }

             if (p_callbacks->on_data(&packet_data[1], bytes_to_write) != 0)
             {
                 p_callbacks->_putc(CAN);
                 p_callbacks->_putc(CAN);
                 p_callbacks->on_finish();
                 return -1;
             }
             bytes_received += bytes_to_write;
             APP_PRINT("YMODEM: Received packet %d. Total bytes: %lu\r\n", packet_data[0], bytes_received);
             p_callbacks->_putc(ACK);
        }
        else /* EOT received */
        {
            APP_PRINT("YMODEM: Received EOT.\r\n");
            break;
        }
    }

    /* --- Finalize Transfer --- */
    APP_PRINT("YMODEM: Sending final ACK.\r\n");
    p_callbacks->_putc(ACK); // Acknowledge the EOT

    APP_PRINT("YMODEM: Sending 'C' for final packet.\r\n");
    p_callbacks->_putc(CRC_CHAR); // Request final null packet
    if (_receive_packet(packet_data, &packet_length, p_callbacks) != 0 || packet_data[0] != 0)
    {
         APP_PRINT("YMODEM WARN: Did not receive final null packet correctly.\r\n");
    }

    APP_PRINT("YMODEM: Sending final ACK.\r\n");
    p_callbacks->_putc(ACK); // Acknowledge final packet
    p_callbacks->on_finish();

    APP_PRINT("YMODEM: Transfer complete.\r\n");
    return 0;
}

static int32_t _receive_packet(uint8_t *p_data, int32_t *p_length, ymodem_callbacks_t *p_callbacks)
{
    uint8_t first_byte = 0;
    uint16_t crc_received;
    uint16_t crc_calculated;
    int32_t payload_size = 0;
    uint8_t header[YMODEM_PACKET_HEADER_SIZE];
    uint32_t timeout = 10000; // 10-second timeout for the first byte of a packet

    *p_length = 0;

    // Wait for a valid start-of-packet character
    while(true)
    {
        if (p_callbacks->_getc(&first_byte, timeout) != 0)
        {
            APP_PRINT("YMODEM ERR: Timeout waiting for packet start.\r\n");
            return -1;
        }

        // We got a byte, subsequent reads should be faster
        timeout = 1000;

        if (first_byte == SOH || first_byte == STX || first_byte == EOT)
        {
            break; // Found a valid start
        }
        // else, discard the byte and loop again
    }


    switch(first_byte)
    {
        case SOH:
            payload_size = YMODEM_SOH_PACKET_SIZE;
            APP_PRINT("YMODEM DBG: SOH received.\r\n");
            break;
        case STX:
            payload_size = YMODEM_STX_PACKET_SIZE;
            APP_PRINT("YMODEM DBG: STX received.\r\n");
            break;
        case EOT:
            return 0;
    }

    /* Read the rest of the packet header */
    if (p_callbacks->_getc(&header[1], 1000) != 0)
    {
        APP_PRINT("YMODEM ERR: Timeout receiving packet header Byte 1.\r\n");
        return -1;
    }
    if (p_callbacks->_getc(&header[2], 1000) != 0)
    {
        APP_PRINT("YMODEM ERR: Timeout receiving packet header Byte 2.\r\n");
        return -1;
    }

    uint8_t packet_num = header[1];
    uint8_t packet_num_inv = header[2];

    APP_PRINT("YMODEM DBG: Received Packet Num: 0x%02X, Inv: 0x%02X\r\n", packet_num, packet_num_inv);
    
    /* HACK: Temporarily disable packet number integrity check for debugging data corruption. */
    //if (packet_num != (uint8_t)(~packet_num_inv))
    //{
    //    APP_PRINT("YMODEM ERR: Packet number integrity check failed.\r\n");
    //    return -1;
    //}

    /* Store packet number as first byte of p_data buffer */
    p_data[0] = packet_num;
    uint8_t *p_payload_data = &p_data[1];
    
    for (int32_t i = 0; i < payload_size; i++)
    {
        if (p_callbacks->_getc(&p_payload_data[i], 1000) != 0)
        {
            APP_PRINT("YMODEM ERR: Timeout receiving payload byte %d.\r\n", i);
            return -1;
        }
    }

    uint8_t crc_bytes[2];
    if (p_callbacks->_getc(&crc_bytes[0], 1000) != 0) return -1;
    if (p_callbacks->_getc(&crc_bytes[1], 1000) != 0) return -1;
    crc_received = (uint16_t)((crc_bytes[0] << 8) | crc_bytes[1]);

    crc_calculated = _crc16_calculate(p_payload_data, (uint32_t)payload_size);

    if (crc_calculated != crc_received)
    {
        APP_PRINT("YMODEM ERR: CRC mismatch. Received: 0x%04X, Calculated: 0x%04X\r\n", crc_received, crc_calculated);
        return -1;
    }

    *p_length = payload_size;
    return 0;
}

static uint16_t _crc16_update(uint16_t crc_in, uint8_t byte)
{
    int i;
    crc_in ^= (uint16_t)byte << 8;
    for (i = 0; i < 8; i++)
    {
        if (crc_in & 0x8000)
        {
            crc_in = (crc_in << 1) ^ 0x1021;
        }
        else
        {
            crc_in = crc_in << 1;
        }
    }
    return crc_in;
}

static uint16_t _crc16_calculate(const uint8_t *p_data, uint32_t size)
{
    uint16_t crc = 0;
    for(uint32_t i = 0; i < size; i++)
    {
        crc = _crc16_update(crc, p_data[i]);
    }
    return crc;
}
