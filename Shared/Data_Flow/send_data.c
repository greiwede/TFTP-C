#include "send_data.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "receive_data.h"
#include "../utils.h"
#include "../file_manipulation.h"
#include "../Packet_Manipulation/packets.h"
#include "../Packet_Manipulation/write_packets.h"
#include "../Packet_Manipulation/read_packets.h"

void send_file(struct request_packet * request, struct socket_meta * socket_information) {
    FILE * fd;
    ssize_t bytes_read;
    int excess_bytes;
    struct data_packet * data_packet;
    uint8_t * data;
    uint8_t * buf;
    uint8_t * netascii_buf;
    uint8_t * excess_queue;
    uint16_t block_number;

    if (strcmp(request->mode, MODE_NETASCII) == 0) {
        fd = fopen(request->file_name, "r");
    } else {
        fd = fopen(request->file_name, "rb");
    }

    if (fd == NULL) {
        printf("Failed to open file - ");
        struct error_packet * error_packet = determine_file_opening_error();
        struct packet_meta * error_meta = build_error_frame(error_packet);
        send_buffer(socket_information, error_meta->ptr, error_meta->length);
        free(error_packet);
        free(error_meta);
        return;
    }

    buf = malloc(sizeof(uint8_t) * PACKET_MAX_LENGTH);
    data = malloc(sizeof(uint8_t) * DATA_MAX_LENGTH);
    // read bytes from file are converted to netascii mode
    // size is 2 * DATA_MAX_LENGTH length because every char could be <CR> and thus replaced by
    // <CR><NUL>
    netascii_buf = malloc(sizeof(uint8_t) * 2 * DATA_MAX_LENGTH);
    // stores bytes that do not fit into data packet after netascii conversion
    excess_queue = malloc(sizeof(uint8_t) * DATA_MAX_LENGTH);

    excess_bytes = 0;
    block_number = 1;
    while ((bytes_read = fread(data, 1, DATA_MAX_LENGTH - excess_bytes, fd))
            == DATA_MAX_LENGTH - excess_bytes) {
        if (strcmp(request->mode, MODE_NETASCII) == 0) {
            bytes_read = handle_netascii_buf(data, bytes_read, netascii_buf, excess_queue, &excess_bytes);
        }
        data_packet = build_data_packet(block_number, data, bytes_read);
        if (send_packet(data, data_packet, block_number, socket_information, buf) == -1) {
            printf("Abort sending file \n");
            free(buf);
            free(excess_queue);
            free(data);
            free(netascii_buf);
            return;
        }
        printf("block no: %i\n", block_number);
        block_number++;

        free_data_packet(data_packet);
    }
    fclose(fd);
    if (strcmp(request->mode, MODE_NETASCII) == 0) {
        bytes_read = handle_netascii_buf(data, bytes_read, netascii_buf, excess_queue, &excess_bytes);
    }
    data_packet = build_data_packet(block_number, data, bytes_read);
    printf("block no: %i\n", block_number);
    if (send_packet(data, data_packet, block_number, socket_information, buf) == -1) {
        printf("Abort sending file \n");
        free(buf);
        free(excess_queue);
        free(data);
        free(netascii_buf);
        return;
    }
    if (bytes_read == DATA_MAX_LENGTH) {
        block_number++;
        data_packet = build_data_packet(block_number, excess_queue, excess_bytes);
        printf("block no: %i\n", block_number);

        if (send_packet(data, data_packet, block_number, socket_information, buf) == -1) {
            printf("Abort sending file \n");
            free(buf);
            free(excess_queue);
            free(data);
            free(netascii_buf);
            return;
        }
        free_data_packet(data_packet);
    }
    printf("file sent \n");

    free(buf);
    free(excess_queue);
    free(data);
    free(netascii_buf);
}

int send_packet(
        uint8_t * data,
        struct data_packet * data_packet,
        uint16_t block_number,
        struct socket_meta * socket_information,
        uint8_t * ack_buf) {

    struct packet_meta * packet_meta;
    struct ack_packet * ack_packet;
    ssize_t sent_bytes;
    ssize_t recv_bytes;
    int times_resent;

    data_packet->opcode = OPCODE_DATA;
    data_packet->block_no = block_number;

    packet_meta = build_data_frame(data_packet);

    times_resent = 0;
    do {
        if ((sent_bytes = send_buffer(socket_information, packet_meta->ptr, packet_meta->length))
                == -1) {
            printf("Problem sending data from inside send packet: %i \n", errno);
            return sent_bytes;
        }
        if ((recv_bytes = receive_buffer(socket_information, ack_buf, PACKET_MAX_LENGTH)) == -1) {
            // timeout occured
            times_resent++;
            continue;
        }

        if ((ack_packet = convert_buf_to_ack_packet(ack_buf, recv_bytes)) == NULL) {
            struct error_packet * error_packet = convert_buf_to_error_packet(ack_buf, recv_bytes);
            if (error_packet == NULL) {
                printf("Unknown error occured receiving acknowledgment");
                return -1;
            }
            printf("Error: %i - %s \n", error_packet->error_code, error_packet->error_message);
            free_error_packet(error_packet);
            return -1;
        }

        if (ack_packet->block_no != block_number) {
            recv_bytes = -1;
        }
        times_resent++;

        free_ack_packet(ack_packet);
    } while (recv_bytes >= 0 && times_resent < MAX_RETRANSMITS);

    if (times_resent == MAX_RETRANSMITS && recv_bytes < 0) {
        printf("Maximum retransmits exceeded");
    }

    free_packet_meta(packet_meta);
    return 1;
}

int send_buffer(
        struct socket_meta * socket_information,
        uint8_t * buf,
        int buf_length) {
    ssize_t sent_bytes;

    if ((sent_bytes = sendto(
                *socket_information->socket,
                buf,
                buf_length,
                0,
                (struct sockaddr *) socket_information->address,
                (socklen_t) socket_information->length))
            == -1) {
        printf("Problem sending data: %i \n", errno);
    }
    return sent_bytes;
}
