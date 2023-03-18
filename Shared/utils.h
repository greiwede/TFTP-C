/**
 * \file            utils.h
 * \brief           uint8_t/uint16_t conversion header file
 */
#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdint.h>

/**
* \brief           struct for socket information
*/
typedef struct socket_meta {
    int * socket;
    struct sockaddr_in * address;
    int length;
} socket_meta;

void free_socket_meta(struct socket_meta * meta);
void uint16_to_uint8_buf(uint16_t value, uint8_t * ptr);
int uint8_buf_to_uint16(uint8_t * ptr);

#endif
