#include "receive_data.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../Packet_Manipulation/packets.h"
#include "../Packet_Manipulation/read_packets.h"
#include "../Packet_Manipulation/write_packets.h"
#include "../utils.h"
#include "../file_manipulation.h"
#include "send_data.h"

void receive_file(struct request_packet * request, struct socket_meta * socket_information) {
    FILE * fd;
    struct data_packet * data_packet;
    uint8_t * data;
    uint8_t * buf;
    uint16_t block_number;

    // TODO: get rid of path for filename
    if (strcmp(request->mode, MODE_NETASCII) == 0) {
        if ((fd = fopen(request->file_name, "w")) == NULL) {
            // TODO: send error
            return;
        }
    } else {
        if ((fd = fopen(request->file_name, "wb")) == NULL) {
            // TODO: send error
            return;
        }
    }

    data = malloc(sizeof(uint8_t) * DATA_MAX_LENGTH);
    buf = malloc(sizeof(uint8_t) * PACKET_MAX_LENGTH);

    block_number = 1;
    uint8_t last_char;
    do {
        if ((data_packet = receive_packet(block_number, socket_information, buf)) == NULL) {
            return;
        }
        if (strcmp(request->mode, MODE_NETASCII) == 0) {
            printf("Converting from netascii \n");
            // store last char from last packet if was CR do not write it
            // if CR -> check first char from new packet for NUL and LF
            int new_length = buf_from_netascii(
                    data_packet->data,
                    data_packet->data_length,
                    &last_char);
            fwrite(data_packet->data, 1, new_length, fd);
        } else {
            fwrite(data_packet->data, 1, data_packet->data_length, fd);
        }
        block_number++;
        printf("Received data length: %i \n", data_packet->data_length);
    } while (data_packet->data_length == 512);
    fclose(fd);
    printf("file received \n");
}

struct data_packet * receive_packet(
        uint16_t block_number,
        struct socket_meta * socket_information,
        uint8_t * data_buf) {

    struct packet_meta * packet_meta;
    struct ack_packet * ack_packet;
    struct data_packet * data_packet;
    ssize_t sent_bytes;
    ssize_t recv_bytes;
    int times_resent;

    ack_packet = build_ack_packet(block_number);
    printf("ack_packet block no: %i \n", ack_packet->block_no);
    packet_meta = build_ack_frame(ack_packet);
    times_resent = 0;
    do {
        recv_bytes = 0;
        if((recv_bytes = receive_buffer(socket_information, data_buf, PACKET_MAX_LENGTH)) == -1) {
            printf("Failed receiving bytes: %i \n", errno);
            continue;
        }
        data_packet = convert_buf_to_data_packet(data_buf, recv_bytes);
        if (data_packet == NULL) {
            struct error_packet * error_packet = convert_buf_to_error_packet(data_buf, recv_bytes);
            if (error_packet == NULL) {
                printf("Unknown error occured receiving data");
                return NULL;
            }
            printf("Error: %i - %s", error_packet->error_code, error_packet->error_message);
            return NULL;
        }

        if ((sent_bytes = send_buffer(
                        socket_information,
                        packet_meta->ptr,
                        packet_meta->length))
                == -1) {
            printf("Failed sending data %i", errno);
            return NULL;
        }

        if (data_packet->block_no != block_number) {
            recv_bytes = -1;
        }
        times_resent++;
    } while (recv_bytes >= 0 && times_resent < MAX_RETRANSMITS);
    free_packet_meta(packet_meta);

    return data_packet;
}

int receive_buffer(
        struct socket_meta * socket_information,
        uint8_t * buf,
        int buf_length) {
    ssize_t recv_bytes;

    memset(buf, 0, buf_length);
    if ((recv_bytes = recvfrom(
                    *socket_information->socket,
                    buf,
                    buf_length,
                    0,
                    (struct sockaddr *) socket_information->address,
                    (socklen_t *) &socket_information->length))
                == -1) {
        printf("Problem receiving data: %i \n", errno);
    }
    return recv_bytes;
}

int set_receiving_timeout(int socket_fd) {
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SECONDS;
    timeout.tv_usec = TIMEOUT_MICRO_SECONDS;

    if (setsockopt (socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                sizeof timeout) < 0) {
        printf("setsockopt failed %i\n", errno);
        return -1;
    }
    return 1;
}
