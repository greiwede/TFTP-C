#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include <errno.h>
#include <pthread.h>

#include "connection_establishment.h"
#include "data_channel.h"
#include "../Shared/Packet_Manipulation/read_packets.h"
#include "../Shared/Packet_Manipulation/packets.h"
#include "../Shared//Data_Flow/receive_data.h"

int main() {
    int control_socket;
    ssize_t recv_len;
    struct socket_meta * control_socket_information = malloc(sizeof(struct socket_meta));

    if ((control_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't create socket \n");
        return -1;
    }
    control_socket_information->socket = &control_socket;

    if (bind_control_socket(control_socket) == -1) {
        return -1;
    }

    while (1) {
        struct sockaddr_in * client_addr = malloc(sizeof(struct sockaddr_in));
        int client_length = sizeof(*client_addr);
        uint8_t * buf = malloc(PACKET_MAX_LENGTH);

        pthread_t data_thread;
        int thread_no;
        struct request_params request_params;

        control_socket_information->address = client_addr;
        control_socket_information->length = client_length;

        memset(buf, 0, PACKET_MAX_LENGTH);
        if ((recv_len = receive_buffer(
                        control_socket_information,
                        buf,
                        PACKET_MAX_LENGTH))
                    == -1) {
            printf("Failed receiving data \n");
            continue;
        }
        printf("received %zu bytes \n", recv_len);

        request_params.buf = buf;
        request_params.buf_length = recv_len;
        request_params.client_addr = client_addr;
        request_params.client_length = client_length;

        thread_no = pthread_create(&data_thread, NULL, handle_request, (void *) &request_params);
    }

    close(control_socket);
}
