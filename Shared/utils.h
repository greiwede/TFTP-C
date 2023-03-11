#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdint.h>

typedef struct socket_meta {
    int * socket;
    struct sockaddr_in * address;
    int length;
} socket_meta;

void uint16_to_uint8_buf(uint16_t value, uint8_t * ptr);
int uint8_buf_to_uint16(uint8_t * ptr);

#endif
