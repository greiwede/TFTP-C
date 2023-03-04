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
#include "send_data.h"

void receive_file(
        struct request_packet * request,
        int * socket,
        struct sockaddr_in * address,
        int address_length) {
    FILE * fd;
    struct data_packet * data_packet;
    uint8_t * data;
    uint8_t * buf;
    uint16_t block_number;

    // FIXME: opening file if does not exist and directory problem
    // if ((fd = fopen(request->file_name, "wb")) == NULL) {
    char * a = "ptest.txt";
    if ((fd = fopen(a, "wb")) == NULL) {
        // TODO: send error
        return;
    }

    data = malloc(sizeof(uint8_t) * DATA_MAX_LENGTH);
    buf = malloc(sizeof(uint8_t) * PACKET_MAX_LENGTH);

    block_number = 1;
    do {
        data_packet = receive_packet(block_number, socket, buf, address, address_length);
        // TODO: handle returned null
        fwrite(data_packet->data, 1, data_packet->data_length, fd);
        block_number++;
    } while (data_packet->data_length == 512);
    fclose(fd);
}

struct data_packet * receive_packet(
        uint8_t block_number,
        int * socket,
        uint8_t * buf,
        struct sockaddr_in * address,
        int address_length) {
    printf("receiving packet \n");

    struct packet_meta * packet_meta;
    struct ack_packet * ack_packet;
    struct data_packet * data_packet;
    size_t sent_bytes;
    ssize_t recv_bytes;
    int times_resent;

    ack_packet = build_ack_packet(block_number);
    packet_meta = build_ack_frame(ack_packet);
    times_resent = 0;
    do {
        printf("receive_buffer");
        recv_bytes = receive_buffer(socket, buf, address, address_length);
        printf("received %zu bytes \n", recv_bytes);
        if (recv_bytes < 0) {
            continue;
        }
        data_packet = convert_buf_to_data_packet(buf, recv_bytes);

        printf("sending ack");
        if ((sent_bytes = send_buffer(
                        socket,
                        packet_meta->ptr,
                        packet_meta->length,
                        address,
                        address_length))
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
        int * socket,
        uint8_t * buf,
        struct sockaddr_in * address,
        int length) {
    ssize_t recv_bytes;

    // TODO: what happens after timeout?
    memset(buf, 0, PACKET_MAX_LENGTH);
    if ((recv_bytes = recvfrom(
                    *socket,
                    buf,
                    PACKET_MAX_LENGTH,
                    0,
                    (struct sockaddr *) address,
                    (socklen_t *) &length))
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
