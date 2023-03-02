#include "send_data.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

#include "../Packet_Manipulation/packets.h"
#include "../Packet_Manipulation/write_packets.h"

void send_data(
        struct request_packet * request,
        int * socket,
        struct sockaddr_in * address,
        int address_length) {
    FILE * fd;
    ssize_t bytes_read;
    uint8_t * data;
    uint16_t block_number;
    struct data_packet data_packet;

    if ((fd = fopen(request->file_name, "rb")) == NULL) {
        // TODO: send error
        return;
    }

    data = malloc(sizeof(uint8_t) * DATA_MAX_LENGTH);
    data_packet.data = data;
    block_number = 1;
    while ((bytes_read = fread(data, 1, DATA_MAX_LENGTH, fd)) == DATA_MAX_LENGTH) {
        struct packet_meta * meta;

        data_packet.opcode = OPCODE_DATA;
        data_packet.block_no = block_number;

        meta = build_data_frame(&data_packet);
        // TODO: start here
        // send!!!
        free_packet_meta(meta);
        block_number++;
    }
}
