#include "data_channel.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdint.h>
#include <errno.h>

#include "../Shared/read_packets.h"
#include "../Shared/packets.h"

void * handle_request(void * request_params) {
    int data_socket;
    size_t recv_len;
    ssize_t sent_bytes;
    int packet_type;

    struct request_params * params = (struct request_params *) request_params;

    if ((data_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't build socket");
        // TODO: stop thread
    }

    for (int i = 0; i < 13; i++) {
        printf("Byte %i: %i \n", i, params->buf[i]);
    }

    packet_type = identify_packet_type(params->buf);
    printf("lol: %i \n", packet_type);

    if (packet_type == OPCODE_RRQ) {
        recv_len = 5;
        if ((sent_bytes = sendto(
                    data_socket,
                    params->buf,
                    recv_len,
                    0,
                    (struct sockaddr *) params->client_addr,
                    (socklen_t) params->client_length))
                == -1) {
            printf("Problem sending stuff: %i \n", errno);
            // TODO: stop thread
        } else {
            printf("valid");
        }
        printf("Sent %zi bytes \n", sent_bytes);
    } else if (packet_type == OPCODE_WRQ) {
    } else {
        // FIXME: send error message
    }

    return NULL;
}
