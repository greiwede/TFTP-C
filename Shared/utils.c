/**
 * \file            utils.c
 * \brief           coversion from 2 uint8_t to uint16_t and vice versa
 */
#include "utils.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/**
 * \brief           convert a uint16_t to two uint8_t and stores them next to each other
 * \param[in]       value: uint16_t value that should be converted
 * \param[in]       ptr: pointer to storage where the 2 uint8_t should be stored
 */
void uint16_to_uint8_buf(uint16_t value, uint8_t * ptr) {
    *(ptr) = value & 0xff;
    *(ptr + 1) = value >> 8;
}


//TODO: warum ist return int und nicht uint16_t
/**
 * \brief           convert two uint8_t to one uint16_t
 * \param[in]       value: Pointer to first uin8_t value that should be converted
 * \return          returns converted value
 */
int uint8_buf_to_uint16(uint8_t * ptr) {
    uint16_t value = 0;
    value += *(ptr);
    value += *(ptr + 1) << 8;
    return value;
}
