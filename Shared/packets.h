#ifndef PACKETS_H_
#define PACKETS_H_

#include <stdint.h>

typedef enum {
    RRQ   = 1,
    WRQ   = 2,
    DATA  = 3,
    ACK   = 4,
    ERROR = 5,
} OPCODE;

typedef struct __attribute__((__packed__)) request_packet {
    uint16_t opcode;
    uint8_t * file_name;
    uint8_t zero_byte_one;
    uint8_t * mode;
    uint8_t zero_byte_two;
} request_packet;

typedef struct __attribute__((__packed__)) data_packet {
    uint16_t opcode;
    uint16_t block_no;
    uint8_t * data;
} data_packet;

typedef struct __attribute__((__packed__)) ack_packet {
    uint16_t opcode;
    uint16_t block_no;
} ack_packet;

typedef struct __attribute__((__packed__)) error_packet {
    uint16_t opcode;
    uint16_t error_code;
    uint8_t * error_message;
    uint8_t zero_byte;
} error_packet;

typedef enum {
    NOT_DEFINED         = 0,
    FILE_NOT_FOUND      = 1,
    ACCESS_VIOLATION    = 2,
    DISK_FULL           = 3,
    ILLEGAL_OPERATION   = 4,
    UNKNOWN_TID         = 5,
    FILE_ALREADY_EXISTS = 6,
    NO_SUCH_USER        = 7
} ERROR_CODE;

#endif
