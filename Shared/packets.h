#ifndef PACKETS_H_
#define PACKETS_H_

#include <stdint.h>

typedef enum {
    OPCODE_RRQ   = 1,
    OPCODE_WRQ   = 2,
    OPCODE_DATA  = 3,
    OPCODE_ACK   = 4,
    OPCODE_ERROR = 5,
} OPCODE;

#define OPCODE_LENGTH 2
#define BLOCK_NO_LENGTH 2
#define ZERO_BYTE_LENGTH 1
#define ERROR_CODE_LENGTH 2

typedef struct packet_meta {
    uint8_t * ptr;
    int length;
} packet_meta;

// FIXME: add length to heap data?
typedef struct __attribute__((__packed__)) request_packet {
    uint16_t opcode;
    char * file_name;
    char * mode;
} request_packet;

typedef struct __attribute__((__packed__)) data_packet {
    uint16_t opcode;
    uint16_t block_no;
    uint8_t * data;
    int data_length; // cannot infer data length from uint8_t pointer
} data_packet;

typedef struct __attribute__((__packed__)) ack_packet {
    uint16_t opcode;
    uint16_t block_no;
} ack_packet;

typedef struct __attribute__((__packed__)) error_packet {
    uint16_t opcode;
    uint16_t error_code;
    char * error_message;
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

request_packet * build_request_packet(uint16_t opcode, char * file_name, char * mode);
data_packet * build_data_packt(uint16_t block_no, uint8_t * data, int data_length);
ack_packet * build_ack_packet(uint16_t block_no);
error_packet * build_error_packet(uint16_t error_code, char * error_message);

#endif
