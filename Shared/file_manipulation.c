/**
 * \file            file_manipulation.c
 * \brief           Everything relating to Netascii conversion and
 *                  determining the error code and message
 */
#include "file_manipulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "Packet_Manipulation/packets.h"

/**
 * \brief           convert a given buffer to netascii mode and stores it in another buffer
 * \note            Please make sure that netascii_buf is at least double the size of data (buffer)
 * \param[in]       data: buffer that should be converted to netascii
 * \param[in]       data_length: size of data_buffer
 * \param[in]       netascii_buf: buffer where result is stored
 * \return          returns the size of the data after the netascii conversion
 */
int buf_to_netascii(uint8_t * data, int data_length, uint8_t * netascii_buf) {
    int index_netascii;
    int index_data;

    index_netascii = 0;
    index_data = 0;
    int replaced = 0;
    while (index_data < data_length) {
        switch (data[index_data]) {
            case ASCII_NL:
                netascii_buf[index_netascii] = ASCII_CR;
                index_netascii++;
                netascii_buf[index_netascii] = ASCII_LF;
                replaced++;
                break;
            case ASCII_CR:
                netascii_buf[index_netascii] = ASCII_CR;
                index_netascii++;
                netascii_buf[index_netascii] = ASCII_NUL;
                replaced++;
                break;
            default:
                netascii_buf[index_netascii] = data[index_data];
                break;
        }
        index_netascii++;
        index_data++;
    }

    return index_netascii;
}

//TODO: how do you call a buffer which is not in netascii mode? origin mode??
/**
 * \brief           convert a given buffer in netascii mode back to origin mode and stores the last
 *                  character of the buffer to variable to check whether <CR><LF> or <CR><NULL>
 *                  were split in between packets
 * \param[in]       buf: buffer that should be converted from netascii
 * \param[in]       buf_length: size of buf
 * \param[in]       last_char: Pointer to a variable to store the last char of the buffer
 * \return          returns the size of the data after the conversion
 */
int buf_from_netascii(uint8_t * buf, int buf_length, uint8_t * last_char) {
    int buf_index;
    uint8_t * local_buf;
    int local_index;

    local_buf = malloc(sizeof(uint8_t) * buf_length);
    buf_index = 0;
    local_index = 0;

    // check whether <CR><LF> or <CR><NULL> were split in between packets
    if (*last_char == ASCII_CR && buf_length > 0) {
        if (buf[0] == ASCII_LF) {
            local_buf[local_index] = ASCII_NL;
        } else { // means -> (next == ASCII_NUL)
            local_buf[local_index] = ASCII_CR;
        }
        local_index++;
        buf_index++;
    }

    int replaced = 0;
    while (buf_index < buf_length) {
        int is_normal_char;
        uint8_t current;
        uint8_t next;

        is_normal_char = 1;
        current = buf[buf_index];
        if (current == ASCII_CR) {
            if (buf_index + 1 < buf_length) {
                next = buf[buf_index + 1];
                if (next == ASCII_LF) {
                    local_buf[local_index] = ASCII_NL;
                } else { // means -> (next == ASCII_NUL)
                    local_buf[local_index] = ASCII_CR;
                }
                buf_index++;
                replaced++;
            }
            is_normal_char = 0;
        }
        if (is_normal_char) {
            local_buf[local_index] = buf[buf_index];
        }
        buf_index++;
        local_index++;
    }

    // safe last_char for checking split with next packet
    *last_char = buf[buf_length - 1];
    if (*last_char == ASCII_CR) {
        local_index--;
    }

    memset(buf, 0, buf_length);
    memcpy(buf, local_buf, local_index);
    free(local_buf);
    return local_index;
}

/**
 * \brief           function that fills buffer with excess queue (if it contains elements) and the
 *                  to netascii mode converted data
 * \note            if data don't fit completly into the buffer, the rest will be stored into the
 *                  excess queue and the number of these elements in excess bytes
 * \param[in]       data: buffer with data before conversion
 * \param[in]       bytes_read: number of bytes that are stored in data
 * \param[in]       netascii_buf: buffer to store temporarly to netascii converted data
 * \param[in]       excess_queue: buffer for excess bytes after coversion
 * \param[in]       excess_bytes: Pointer to number of excess bytes
 * \return          returns bytes that should be send
 */
int handle_netascii_buf(
        uint8_t * data,
        int bytes_read,
        uint8_t * netascii_buf,
        uint8_t * excess_queue,
        int * excess_bytes) {

    int netascii_buf_length;
    int send_bytes;
    int available_bytes = DATA_MAX_LENGTH - *excess_bytes;

    netascii_buf_length = buf_to_netascii(data, bytes_read, netascii_buf);
    memcpy(data, excess_queue, *excess_bytes);
    memcpy(data + *excess_bytes, netascii_buf, available_bytes);
    if (netascii_buf_length > available_bytes) {
        *excess_bytes = netascii_buf_length - available_bytes;
        memcpy(excess_queue, netascii_buf + available_bytes, *excess_bytes);
        send_bytes = DATA_MAX_LENGTH;
    } else {
        send_bytes = *excess_bytes + netascii_buf_length;
        *excess_bytes = 0;
    }
    return send_bytes;
}

/**
 * \brief           checks errno and determine error code and message based on that
 * \return          error packet
 */
struct error_packet * determine_file_opening_error() {
    char * reason;
    int code;
    switch (errno) {
        case ENOENT:
            printf("File not found\n");
            reason = "File not found";
            code = EC_FILE_NOT_FOUND;
            break;
        case EACCES:
            printf("Permission denied\n");
            reason = "Permission denied";
            code = EC_ACCESS_VIOLATION;
            break;
        case ENOMEM:
            printf("Insufficient memory\n");
            reason = "Insufficient memory";
            code = EC_ALLOCATION_EXCEEDED;
            break;
        case EINVAL:
            printf("Invalid argument\n");
            reason = "Invalid argument";
            code = EC_UNKNOWN_TRANSFER_ID;
            break;
        case EBUSY:
            printf("Device or resource busy\n");
            reason = "Device or resource busy";
            code = EC_ALLOCATION_EXCEEDED;
            break;
        case EIO:
            printf("Input/output error\n");
            reason = "Input/output error";
            code = EC_NOT_DEFINIED;
            break;
        case ENXIO:
            printf("No such device or address\n");
            reason = "No such device or address";
            code = EC_NOT_DEFINIED;
            break;
        default:
            printf("Unknown error\n");
            reason = "Unknown error";
            code = EC_NOT_DEFINIED;
            break;
    }
    return build_error_packet(code, reason);
}
