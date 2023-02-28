#include "packets.h"

#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "../Shared/utils.h"

void free_packet_meta(struct packet_meta * packet) {
    free(packet->ptr);
    free(packet);
}

void free_request_packet(struct request_packet * packet) {
    free(packet->file_name);
    // note: mode does not need to be deallocated as its a pointer to a static string
    free(packet);
}

void free_data_packet(struct data_packet * packet) {
    free(packet->data);
    free(packet);
}

void free_ack_packet(struct ack_packet * packet) {
    free(packet);
}

void free_error_packet(struct error_packet * packet) {
    free(packet->error_message);
    free(packet);
}

int identify_packet_type(uint8_t * frame) {
    uint16_t opcode = ntohs(uint8_buf_to_uint16(frame));
    printf("Opcode: %i \n",opcode);
    if (opcode >= 0 && opcode <= 5) {
        return opcode;
    } else {
        return -1;
    }
}

// TODO: do we need structs for sending packet?
// frame -> struct - entpacken
// values -> frame - einpacken

request_packet * build_request_packet(uint16_t opcode, char * file_name, char * mode) {
    struct request_packet * packet = malloc(sizeof(struct request_packet));
    packet->opcode = opcode;
    packet->file_name = file_name;
    packet->mode = mode;
    return packet;
}

data_packet * build_data_packet(uint16_t block_no, uint8_t * data, int data_length) {
    struct data_packet * packet = malloc(sizeof(struct data_packet));
    packet->opcode = OPCODE_DATA;
    packet->block_no = block_no;
    packet->data = data;
    packet->data_length = data_length;
    return packet;
}

ack_packet * build_ack_packet(uint16_t block_no) {
    struct ack_packet * packet = malloc(sizeof(struct ack_packet));
    packet->opcode = OPCODE_ACK;
    packet->block_no = block_no;
    return packet;
}

error_packet * build_error_packet(uint16_t error_code, char * error_message) {
    struct error_packet * packet = malloc(sizeof(struct error_packet));
    packet->opcode = OPCODE_ERROR;
    packet->error_code = error_code;
    packet->error_message = error_message;
    return packet;
}

packet_meta * build_request_frame(request_packet * information) {
    int file_name_length = strlen(information->file_name);
    int mode_length = strlen(information->mode);

    struct packet_meta * meta = malloc(sizeof(struct packet_meta));
    meta->length = OPCODE_LENGTH + file_name_length
        + ZERO_BYTE_LENGTH + mode_length + ZERO_BYTE_LENGTH;
    meta->ptr = malloc(meta->length);
    memset(meta->ptr, 0, meta->length);

    int offset = 0;
    uint16_to_uint8_buf(htons(information->opcode), meta->ptr);
    offset += OPCODE_LENGTH;
    memcpy(meta->ptr + offset, information->file_name, file_name_length);
    offset += file_name_length;
    memset(meta->ptr + offset, 0, ZERO_BYTE_LENGTH);
    offset += ZERO_BYTE_LENGTH;
    memcpy(meta->ptr + offset, information->mode, mode_length);
    offset += mode_length;
    memset(meta->ptr + offset, 0, ZERO_BYTE_LENGTH);
    return meta;
}

packet_meta * build_data_frame(data_packet * information) {
    struct packet_meta * meta = malloc(sizeof(struct packet_meta));
    meta->length = OPCODE_LENGTH + BLOCK_NO_LENGTH + information->data_length;
    meta->ptr = malloc(meta->length);

    int offset = 0;
    uint16_to_uint8_buf(htons(information->opcode), meta->ptr);
    offset += OPCODE_LENGTH;
    uint16_to_uint8_buf(htons(information->block_no), meta->ptr + offset);
    offset += BLOCK_NO_LENGTH;
    memcpy(meta->ptr + offset, information->data, information->data_length);

    return meta;
}

packet_meta * build_ack_frame(ack_packet * information) {
    struct packet_meta * meta = malloc(sizeof(struct packet_meta));
    meta->length = OPCODE_LENGTH + BLOCK_NO_LENGTH;
    meta->ptr = malloc(meta->length);

    int offset = 0;
    uint16_to_uint8_buf(htons(information->opcode), meta->ptr);
    offset += OPCODE_LENGTH;
    uint16_to_uint8_buf(htons(information->block_no), meta->ptr + offset);

    return meta;
}

packet_meta * build_error_frame(error_packet * information) {
    int error_message_length = strlen(information->error_message);

    struct packet_meta * meta = malloc(sizeof(struct packet_meta));
    meta->length = OPCODE_LENGTH + BLOCK_NO_LENGTH + error_message_length + ZERO_BYTE_LENGTH;
    meta->ptr = malloc(meta->length);

    int offset = 0;
    uint16_to_uint8_buf(htons(information->opcode), meta->ptr);
    offset += OPCODE_LENGTH;
    uint16_to_uint8_buf(htons(information->error_code), meta->ptr + offset);
    offset += ERROR_CODE_LENGTH;
    memcpy(meta->ptr + offset, information->error_message, error_message_length);
    offset += error_message_length;
    memset(meta->ptr + offset, 0, ZERO_BYTE_LENGTH);
    return meta;
}
