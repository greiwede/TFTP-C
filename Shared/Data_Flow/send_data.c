#include "send_data.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../Packet_Manipulation/packets.h"
#include "../Packet_Manipulation/write_packets.h"
#include "../Packet_Manipulation/read_packets.h"
#include "receive_data.h"

void send_file(
        struct request_packet * request,
        int * socket,
        struct sockaddr_in * address,
        int address_length) {
    FILE * fd;
    ssize_t bytes_read;
    struct data_packet * data_packet;
    uint8_t * data;
    uint8_t * buf;
    uint16_t block_number;


    if ((fd = fopen(request->file_name, "rb")) == NULL) {
        // TODO: send error
        return;
    }

    data = malloc(sizeof(uint8_t) * DATA_MAX_LENGTH);
    buf = malloc(sizeof(uint8_t) * PACKET_MAX_LENGTH);
    block_number = 1;
    while ((bytes_read = fread(data, 1, DATA_MAX_LENGTH, fd)) == DATA_MAX_LENGTH) {
        data_packet = build_data_packet(block_number, data, bytes_read);
        if (send_packet(data, data_packet, block_number, socket, buf, address, address_length)
                == -1) {
            return;
        }
        printf("block no %i\n", block_number);
        block_number++;
    }
    fclose(fd);
    // TODO: how does fread act if file already finished?
    if (send_packet(data, data_packet, block_number, socket, buf, address, address_length)
            == -1) {
        return;
    }
}

int send_packet(
        uint8_t * data,
        struct data_packet * data_packet,
        uint16_t block_number,
        int * socket,
        uint8_t * ack_buf,
        struct sockaddr_in * address,
        int address_length) {

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
        printf("Meta length: %i \n", packet_meta->length);
        if ((sent_bytes = send_buffer(
                        socket,
                        packet_meta->ptr,
                        packet_meta->length,
                        address,
                        address_length))
                == -1) {
            printf("Problem sending data from inside send packet: %i \n", errno);
            return sent_bytes;
        }

        recv_bytes = receive_buffer(socket, ack_buf, address, address_length);
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
        int * socket,
        uint8_t * buf,
        int buf_length,
        struct sockaddr_in * address,
        int address_length) {
    size_t sent_bytes;

    printf("how long is my puffer: %i \n", buf_length);
    if ((sent_bytes = sendto(
                *socket,
                buf,
                buf_length,
                0,
                (struct sockaddr *) address,
                (socklen_t) address_length))
            == -1) {
        printf("Problem sending data: %i \n", errno);
    }
    return sent_bytes;
}
