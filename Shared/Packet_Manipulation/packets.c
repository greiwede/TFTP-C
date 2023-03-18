/**
 * \file            packets.c
 * \brief           Build and free packets
 */
#include "packets.h"

#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "../utils.h"
#include "read_packets.h"

/**
 * \brief           This function frees all the manually allocated memory for struct packet_meta
 * \param[in]       packet: struct where allocated memory should be freed
 */
void free_packet_meta(struct packet_meta * packet) {
    free(packet->ptr);
    free(packet);
}

/**
 * \brief           This function frees all the manually allocated memory for struct request_packet
 * \param[in]       packet: struct where allocated memory should be freed
 */
void free_request_packet(struct request_packet * packet) {
    free(packet->file_name);
    // note: mode does not need to be deallocated as its a pointer to a static string
    free(packet);
}

/**
 * \brief           This function frees all the manually allocated memory for struct data_packet
 * \param[in]       packet: struct where allocated memory should be freed
 */
void free_data_packet(struct data_packet * packet) {
    free(packet->data);
    free(packet);
}

/**
 * \brief           This function frees all the manually allocated memory for struct ack_packet
 * \param[in]       packet: struct where allocated memory should be freed
 */
void free_ack_packet(struct ack_packet * packet) {
    free(packet);
}

/**
 * \brief           This function frees all the manually allocated memory for struct error_packet
 * \param[in]       packet: struct where allocated memory should be freed
 */
void free_error_packet(struct error_packet * packet) {
    free(packet->error_message);
    free(packet);
}

// XXX: add explicit casting when malloc

/**
 * \brief           This function build a request packet with the values from the parameter
 * \param[in]       opcode: Opcode (RRQ, WRQ) for the request packet
 * \param[in]       file_name: filename for the request packet
 * \param[in]       mode: mode for the request packet
 */
request_packet * build_request_packet(uint16_t opcode, char * file_name, char * mode) {
    struct request_packet * packet = malloc(sizeof(struct request_packet));
    packet->opcode = opcode;
    packet->file_name = file_name;
    packet->mode = mode;
    return packet;
}

/**
 * \brief           This function build a data packet with the values from the parameter
 * \param[in]       block_no: block_number for the data packet
 * \param[in]       data: data for the data packet
 * \param[in]       data_length: how much data should be considered for the data packet
 */
data_packet * build_data_packet(uint16_t block_no, uint8_t * data, int data_length) {
    struct data_packet * packet = malloc(sizeof(struct data_packet));
    packet->opcode = OPCODE_DATA;
    packet->block_no = block_no;
    packet->data = malloc(data_length);
    memcpy(packet->data, data, data_length);
    packet->data_length = data_length;
    return packet;
}

/**
 * \brief           This function build an acknoledge packet with the values from the parameter
 * \param[in]       block_no: block_number for the ack packet
 */
ack_packet * build_ack_packet(uint16_t block_no) {
    struct ack_packet * packet = malloc(sizeof(struct ack_packet));
    packet->opcode = OPCODE_ACK;
    packet->block_no = block_no;
    return packet;
}

/**
 * \brief           This function build an error packet with the values from the parameter
 * \param[in]       error_code: error code for the error packet
 * \param[in]       error_message: error message for the error packet
 */
error_packet * build_error_packet(uint16_t error_code, char * error_message) {
    struct error_packet * packet = malloc(sizeof(struct error_packet));
    packet->opcode = OPCODE_ERROR;
    packet->error_code = error_code;
    packet->error_message = error_message;
    return packet;
}
