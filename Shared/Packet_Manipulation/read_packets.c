#include "read_packets.h"

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>

#include "packets.h"
#include "../utils.h"

int identify_packet_type(uint8_t * frame) {
    uint16_t opcode = ntohs(uint8_buf_to_uint16(frame));
    printf("Opcode: %i \n",opcode);
    if (opcode >= 0 && opcode <= 5) {
        return opcode;
    } else {
        return -1;
    }
}


struct request_packet * convert_buf_to_request_packet(uint8_t * buf, ssize_t received_bytes) {
    int packet_type;
    struct request_packet * request;
    int offset;

    char * strings;
    char * first_string;
    char * second_string;

    if (received_bytes < REQUEST_PACKET_MIN_LENGTH) {
        return NULL;
    }
    packet_type = identify_packet_type(buf);
    if (packet_type != OPCODE_RRQ && packet_type != OPCODE_WRQ) {
        return NULL;
    }

    offset = 0;
    request = malloc(sizeof(struct request_packet));
    request->opcode = packet_type;
    offset += OPCODE_LENGTH;
    strings = (char *) buf;
    first_string = strtok(strings + offset, "0");
    request->file_name = first_string;
    offset += strlen(first_string) + ZERO_BYTE_LENGTH;
    second_string = strtok(strings + offset, "0");
    request->mode = second_string;
    return request;
}

struct data_packet * convert_buf_to_data_packet(uint8_t * buf, ssize_t received_bytes) {
    int packet_type;
    struct data_packet * data_packet;
    int offset;
    int data_length;

    if (received_bytes < DATA_PACKET_MIN_LENGTH) {
        return NULL;
    }
    packet_type = identify_packet_type(buf);
    if (packet_type != OPCODE_DATA) {
        return NULL;
    }

    offset = 0;
    data_packet = malloc(sizeof(struct data_packet));
    data_packet->opcode = packet_type;
    offset += OPCODE_LENGTH;
    data_packet->block_no = uint8_buf_to_uint16(buf + offset);
    offset += BLOCK_NO_LENGTH;
    data_length = received_bytes - offset;
    data_packet->data = malloc(sizeof(uint8_t) * data_length);
    memcpy(data_packet->data, buf + offset, data_length);

    return data_packet;
}

// TODO: check if the following is correct
struct ack_packet * convert_buf_to_ack_packet(uint8_t * buf, ssize_t received_bytes) {
    int packet_type;
    struct ack_packet * ack_packet;
    int offset;

    if (received_bytes < ACK_PACKET_LENGTH) {
        return NULL;
    }
    packet_type = identify_packet_type(buf);
    if (packet_type != OPCODE_ACK) {
        return NULL;
    }
    offset = 0;
    ack_packet = malloc(sizeof(struct ack_packet));
    ack_packet->opcode = packet_type;
    offset += OPCODE_LENGTH;
    ack_packet->block_no = uint8_buf_to_uint16(buf + offset);

    return ack_packet;
}

struct error_packet * convert_buf_to_error_packet(uint8_t * buf, ssize_t received_bytes) {
    int packet_type;
    struct error_packet * error_packet;
    int offset;
    // TODO: besseren Namen für strings
    char * strings;

    if (received_bytes < ERROR_PACKET_MIN_LENGTH) {
        return NULL;
    }
    packet_type = identify_packet_type(buf);
    if (packet_type != OPCODE_ERROR) {
        return NULL;
    }

    offset = 0;
    error_packet = malloc(sizeof(struct request_packet));
    error_packet->opcode = packet_type;
    offset += OPCODE_LENGTH;
    //TODO: is the following correct?
    error_packet->error_code = uint8_buf_to_uint16(buf + offset);
    offset += ERROR_CODE_LENGTH;
    strings = (char *) buf;
    error_packet->error_message = strtok(strings + offset, "0");

    return error_packet;
}
