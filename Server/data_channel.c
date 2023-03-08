#include "data_channel.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

#include "data_channel.h"
#include "../Shared/utils.h"
#include "../Shared/Packet_Manipulation/packets.h"
#include "../Shared/Packet_Manipulation/read_packets.h"
#include "../Shared/Packet_Manipulation/write_packets.h"
#include "../Shared/Data_Flow/send_data.h"
#include "../Shared/Data_Flow/receive_data.h"

void * handle_request(void * request_params) {
    int data_socket;
    struct socket_meta * socket_information;
    struct request_params * params;
    struct request_packet * packet;

    params = (struct request_params *) request_params;
    socket_information = malloc(sizeof(struct socket_meta));
    socket_information->socket = &data_socket;
    socket_information->address = params->client_addr;
    socket_information->length = params->client_length;

    if ((data_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't build socket");
        pthread_exit(NULL);
    }
    printf("data socket: %i \n", data_socket);
    set_receiving_timeout(data_socket);

    packet = convert_buf_to_request_packet(params->buf, params->buf_length);

    if (packet->opcode == OPCODE_RRQ) {
        send_file(packet, socket_information);
    } else if (packet->opcode == OPCODE_WRQ) {
        struct ack_packet * first_ack = build_ack_packet(0);
        struct packet_meta * meta = build_ack_frame(first_ack);
        int sent_bytes;
        if ((sent_bytes = send_buffer(
                        socket_information,
                        meta->ptr,
                        meta->length))
                == -1) {
            printf("data connection coouldn't be established %i \n", errno);
        } else {
            //TODO: Error Handling??
            receive_file(packet, socket_information);
        }
    } else {
        // FIXME: send error message
    }
    pthread_exit(NULL);
    return NULL;
}
