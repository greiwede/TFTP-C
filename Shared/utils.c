#include "utils.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void uint16_to_uint8_buf(uint16_t value, uint8_t * ptr) {
    *(ptr) = value & 0xff;
    *(ptr + 1) = value >> 8;
}

int uint8_buf_to_uint16(uint8_t * ptr) {
    uint16_t value = 0;
    value += *(ptr);
    value += *(ptr + 1) << 8;
    return value;
}
