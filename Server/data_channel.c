#include "data_channel.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>

#include "../Shared/Packet_Manipulation/read_packets.h"
#include "../Shared/Packet_Manipulation/packets.h"
#include "../Shared/Data_Flow/send_data.h"
#include "data_channel.h"

void * handle_request(void * request_params) {
    int data_socket;
    size_t recv_len;
    ssize_t sent_bytes;

    struct request_params * params;
    struct request_packet * packet;

    params = (struct request_params *) request_params;

    if ((data_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't build socket");
        pthread_exit(NULL);
    }

    packet = convert_buf_to_request_packet(params->buf, params->buf_length);

    if (packet->opcode == OPCODE_RRQ) {
        send_data(packet, &data_socket, params->client_addr, params->client_length);
        // TODO: send_data(file, socket, address)
    } else if (packet->opcode == OPCODE_WRQ) {
        // TODO: receive_data(file, socket, address)
    } else {
        // FIXME: send error message
    }

    return NULL;
}
