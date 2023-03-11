#include "file_manipulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Packet_Manipulation/packets.h"

// FIXME: make sure each char is actually ascii
int buf_to_netascii(int * excess_bytes, uint8_t * data, int data_length, uint8_t * excess_queue) {
    int index_netascii;
    int index_data;
    int index_queue;
    uint8_t * netascii_buf;

    memcpy(data, excess_queue, *excess_bytes);
    memset(excess_queue, 0, *excess_bytes);
    netascii_buf = malloc(sizeof(uint8_t) * DATA_MAX_LENGTH);

    index_netascii = 0;
    index_data = 0;
    int replaced = 0;
    while (index_netascii < data_length) {
        switch (data[index_data]) {
            case ASCII_NL:
                netascii_buf[index_netascii] = ASCII_CR;
                index_netascii++;
                netascii_buf[index_netascii] = ASCII_LF;
                (*excess_bytes)++;
                replaced++;
                break;
            case ASCII_CR:
                netascii_buf[index_netascii] = ASCII_CR;
                index_netascii++;
                netascii_buf[index_netascii] = ASCII_NUL;
                (*excess_bytes)++;
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

    index_queue = 0;
    while (index_data < data_length) {
        excess_queue[index_queue] = data[index_data];
        index_queue++;
        index_data++;
    }

    memcpy(data, netascii_buf, index_netascii + 1);
    free(netascii_buf);
    return index_netascii;
}

int buf_from_netascii(uint8_t * buf, int buf_length) {
    int buf_index;
    uint8_t * local_buf;
    int local_index;

    local_buf = malloc(sizeof(uint8_t) * buf_length);
    buf_index = 0;
    local_index = 0;

    int replaced = 0;
    while (buf_index < buf_length) {
        int is_normal_char;
        uint8_t current;
        uint8_t next;

        is_normal_char = 1;
        current = buf[buf_index];
        if (buf_index + 1 < buf_length) {
            next = buf[buf_index + 1];
            if (current == ASCII_CR) {
                if (next == ASCII_LF) {
                    local_buf[local_index] = ASCII_NL;
                } else { // means -> (next == ASCII_NUL)
                    local_buf[local_index] = ASCII_CR;
                }
                local_index++;
                is_normal_char = 0;
                replaced++;
            }
        }
        if (is_normal_char) {
            local_buf[local_index] = buf[buf_index];
        }
        buf_index++;
        local_index++;
    }
    printf("Replaced chars %i \n", replaced);

    memset(buf, 0, buf_length);
    memcpy(buf, local_buf, local_index);
    free(local_buf);
    return local_index;
}
