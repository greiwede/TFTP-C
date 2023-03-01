#include "data_channel.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>

#include "../Shared/read_packets.h"
#include "../Shared/packets.h"
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
        ssize_t read_bytes;
        FILE * fd;

        if ((fd = fopen(packet->file_name, "rb")) == NULL) {
            // send error
            pthread_exit(NULL);
        }
        do {
            // int read_bytes = read(fd, );
            // read file -> 512 bytes or rest
            // pack into buffer
            // send buffer
            // receive ack
        } while (read_bytes >= 512);

        //recv_len = 5;
        //if ((sent_bytes = sendto(
        //            data_socket,
        //            params->buf,
        //            recv_len,
        //            0,
        //            (struct sockaddr *) params->client_addr,
        //            (socklen_t) params->client_length))
        //        == -1) {
        //    printf("Problem sending stuff: %i \n", errno);
        //    pthread_exit(NULL);
        //}
        //printf("Sent %zi bytes \n", sent_bytes);
    } else if (packet->opcode == OPCODE_WRQ) {
    } else {
        // FIXME: send error message
    }

    return NULL;
}
