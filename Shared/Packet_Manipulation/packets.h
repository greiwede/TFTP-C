#ifndef PACKETS_H_
#define PACKETS_H_

#include <stdint.h>

/*
 * Default port for control channel is 69
 */
#define TFTP_PORT 8800


/*
 * for packet max total packet length
 */
#define PACKET_MAX_LENGTH 516

#define REQUEST_PACKET_MIN_LENGTH 6
#define DATA_PACKET_MIN_LENGTH 4
#define ACK_PACKET_LENGTH 4
#define ERROR_PACKET_MIN_LENGTH 5

#define DATA_MAX_LENGTH 512

typedef enum {
    OPCODE_RRQ   = 1,
    OPCODE_WRQ   = 2,
    OPCODE_DATA  = 3,
    OPCODE_ACK   = 4,
    OPCODE_ERROR = 5,
} OPCODE;

typedef enum {
    EC_NOT_DEFINIED = 0,
    EC_FILE_NOT_FOUND = 1,
    EC_ACCESS_VIOLATION = 2,
    EC_ALLOCATION_EXCEEDED = 3,
    EC_ILLEGAL_TFTP_OP = 4,
    EC_UNKNOWN_TRANSFER_ID = 5,
    EC_FILE_ALREADY_EXISTS = 6,
    EC_NO_SUCH_USER = 7,
} ERROR_CODE;

#define OPCODE_LENGTH 2
#define BLOCK_NO_LENGTH 2
#define ZERO_BYTE_LENGTH 1
#define ERROR_CODE_LENGTH 2

#define MAX_FILE_NAME_LENGTH 100

#define MODE_NETASCII "netascii"
#define MODE_OCTET "octet"
// deprecated:
#define MODE_MAIL "mail"

typedef struct packet_meta {
    uint8_t * ptr;
    int length;
} packet_meta;

typedef struct __attribute__((__packed__)) request_packet {
    uint16_t opcode;
    char * file_name;
    char * mode;
    // note: mode does not need to be deallocated as its a pointer to a static string
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

void free_packet_meta(struct packet_meta * packet);
void free_request_packet(struct request_packet * packet);
void free_data_packet(struct data_packet * packet);
void free_ack_packet(struct ack_packet * packet);
void free_error_packet(struct error_packet * packet);

request_packet * build_request_packet(uint16_t opcode, char * file_name, char * mode);
data_packet * build_data_packet(uint16_t block_no, uint8_t * data, int data_length);
ack_packet * build_ack_packet(uint16_t block_no);
error_packet * build_error_packet(uint16_t error_code, char * error_message);

#endif
