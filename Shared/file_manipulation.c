#include "file_manipulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Packet_Manipulation/packets.h"

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
    printf("Replaced chars: %i \n", replaced);

    return index_netascii;
}

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
    printf("Replaced chars %i \n", replaced);

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

int handle_netascii_buf(uint8_t * data, int bytes_read, uint8_t * netascii_buf, uint8_t * excess_queue, int * excess_bytes) {
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
