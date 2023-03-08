#include "send_data.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "../Packet_Manipulation/packets.h"
#include "../Packet_Manipulation/write_packets.h"
#include "../Packet_Manipulation/read_packets.h"
#include "receive_data.h"

//TODO: warum muss hier nicht utils importiert werden?

void send_file(
        struct request_packet * request,
        struct socket_meta * socket_information) {
    FILE * fd;
    ssize_t bytes_read;
    struct data_packet * data_packet;
    uint8_t * data;
    uint8_t * buf;
    uint16_t block_number;

    // FIXME:
    if (strcmp(request->mode, MODE_NETASCII)) {
        if ((fd = fopen(request->file_name, "r")) == NULL) {
            // TODO: send error
            return;
        }
    } else {
        if ((fd = fopen(request->file_name, "rb")) == NULL) {
            // TODO: send error
            return;
        }
    }


    data = malloc(sizeof(uint8_t) * DATA_MAX_LENGTH);
    buf = malloc(sizeof(uint8_t) * PACKET_MAX_LENGTH);
    block_number = 1;
    while ((bytes_read = fread(data, 1, DATA_MAX_LENGTH, fd)) == DATA_MAX_LENGTH) {
        data_packet = build_data_packet(block_number, data, bytes_read);
        if (send_packet(data, data_packet, block_number, socket_information, buf)
                == -1) {
            return;
        }
        printf("block no %i\n", block_number);
        printf("bytes read %zu \n", bytes_read);
        block_number++;
    }
    fclose(fd);
    data_packet = build_data_packet(block_number, data, bytes_read);
    if (send_packet(data, data_packet, block_number, socket_information, buf)
            == -1) {
        return;
    }
    printf("data Packet size: %i \n", data_packet->data_length);
    printf("bytes read %zu \n", bytes_read);
    printf("file sent \n");
}

int send_packet(
        uint8_t * data,
        struct data_packet * data_packet,
        uint16_t block_number,
        struct socket_meta * socket_information,
        uint8_t * ack_buf) {

    struct packet_meta * packet_meta;
    struct ack_packet * ack_packet;
    size_t sent_bytes;
    ssize_t recv_bytes;
    int times_resent;

    data_packet->opcode = OPCODE_DATA;
    data_packet->block_no = block_number;

    packet_meta = build_data_frame(data_packet);

    times_resent = 0;
    do {
        if ((sent_bytes = send_buffer(
                        socket_information,
                        packet_meta->ptr,
                        packet_meta->length))
                == -1) {
            printf("Problem sending data from inside send packet: %i \n", errno);
            return sent_bytes;
        }
        //FIXME: check if recv_bytes == -1
        recv_bytes = receive_buffer(socket_information, ack_buf, ACK_PACKET_LENGTH);
        printf("received %zu bytes \n", recv_bytes);

        if ((ack_packet = convert_buf_to_ack_packet(ack_buf, recv_bytes)) == NULL) {
            printf("Not an ACK packet \n");
            // TODO: send error
            return -1;
        }

        if (ack_packet->block_no != block_number) {
            recv_bytes = -1;
        }
        times_resent++;
    } while (recv_bytes >= 0 && times_resent < MAX_RETRANSMITS);
    free_packet_meta(packet_meta);
    return 1;
}

int send_buffer(
        struct socket_meta * socket_information,
        uint8_t * buf,
        int buf_length) {
    size_t sent_bytes;

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
