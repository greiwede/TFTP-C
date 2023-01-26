#ifndef PACKETS_H_
#define PACKETS_H_

#include <stdint.h>

/*
 * Default port for control channel is 69
 */
#define TFTP_PORT 8800

/*
 *
 * TODO: fix to correct size
 */
#define REQUEST_PACKET_LENGTH 100

/*
 * for data packet max total packet length 520
 * and max 512 of actual data and 8 byte header
 */
#define DATA_PACKET_MAX_LENGTH 516

#define ACK_PACKET_LENGTH 4

/*
 *
 * TODO: fix to correct size
 */
#define ERROR_PACKET_LENGTH 4

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

#define MAX_FILE_NAME_LENGTH 100

#define MODE_NETASCII "netascii"
#define MODE_OCTET "octet"
#define MODE_MAIL "mail"

typedef struct packet_meta {
    uint8_t * ptr;
    int length;
} packet_meta;

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

void free_packet_meta(struct packet_meta * packet);
void free_request_packet(struct request_packet * packet);
void free_data_packet(struct data_packet * packet);
void free_ack_packet(struct ack_packet * packet);
void free_error_packet(struct error_packet * packet);

int identify_packet_type(uint8_t * frame);

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

packet_meta * build_request_frame(request_packet * information);
packet_meta * build_data_frame(data_packet * information);
packet_meta * build_ack_frame(ack_packet * information);
packet_meta * build_error_frame(error_packet * information);

#endif
