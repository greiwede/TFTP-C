#include "packets.h"

#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

request_packet * build_request_packet(uint16_t opcode, char * file_name, char * mode) {
    struct request_packet * packet = malloc(sizeof(struct request_packet));
    packet->opcode = htons(opcode);
    packet->file_name = file_name;
    packet->mode = mode;
    return packet;
}

data_packet * build_data_packt(uint16_t block_no, uint8_t * data, int data_length) {
    struct data_packet * packet = malloc(sizeof(struct data_packet));
    packet->opcode = htons(OPCODE_DATA);
    packet->block_no = htons(block_no);
    packet->data = data;
    packet->data_length = data_length;
    return packet;
}

ack_packet * build_ack_packet(uint16_t block_no) {
    struct ack_packet * packet = malloc(sizeof(struct ack_packet));
    packet->opcode = htons(OPCODE_ACK);
    packet->block_no = htons(block_no);
    return packet;
}

error_packet * build_error_packet(uint16_t error_code, char * error_message) {
    struct error_packet * packet = malloc(sizeof(struct error_packet));
    packet->opcode = htons(OPCODE_ERROR);
    packet->error_code = htons(error_code);
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

    int offset = 0;
    *(meta->ptr) = information->opcode;
    offset += OPCODE_LENGTH;
    memcpy(meta->ptr, information->file_name, file_name_length);
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
    *(meta->ptr) = information->opcode;
    offset += OPCODE_LENGTH;
    *(meta->ptr) = information->block_no;
    offset += BLOCK_NO_LENGTH;
    memcpy(meta->ptr, information->data, information->data_length);

    return meta;
}

packet_meta * build_ack_frame(ack_packet * information) {
    struct packet_meta * meta = malloc(sizeof(struct packet_meta));
    meta->length = OPCODE_LENGTH + BLOCK_NO_LENGTH;
    meta->ptr = malloc(meta->length);

    int offset = 0;
    *(meta->ptr) = information->opcode;
    offset += OPCODE_LENGTH;
    *(meta->ptr) = information->block_no;

    return meta;
}

packet_meta * build_error_frame(error_packet * information) {
    int error_message_length = strlen(information->error_message);

    struct packet_meta * meta = malloc(sizeof(struct packet_meta));
    meta->length = OPCODE_LENGTH + BLOCK_NO_LENGTH + error_message_length + ZERO_BYTE_LENGTH;
    meta->ptr = malloc(meta->length);

    int offset = 0;
    *(meta->ptr) = information->opcode;
    offset += OPCODE_LENGTH;
    *(meta->ptr) = information->error_code;
    offset += ERROR_CODE_LENGTH;
    memcpy(meta->ptr, information->error_message, error_message_length);
    offset += error_message_length;
    memset(meta->ptr, 0, ZERO_BYTE_LENGTH);
    return meta;
}
