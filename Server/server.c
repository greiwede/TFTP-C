#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "connection_establishment.h"
#include "../Shared/read_packets.h"
#include "../Shared/constants.h"
#include "../Shared/packets.h"

int main() {
    int control_socket;
    u_int8_t buf[REQUEST_PACKET_LENGTH];
    int recv_len;

    int data_socket;
    struct sockaddr_in client_addr;
    int client_length = sizeof(client_addr);

    if ((control_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't create socket \n");
        return -1;
    }

    if (bind_control_socket(control_socket) == -1) {
        return -1;
    }

    while (1) {
        memset(buf, 0, REQUEST_PACKET_LENGTH);
        if ((recv_len = recvfrom(
                        control_socket,
                        buf,
                        REQUEST_PACKET_LENGTH,
                        0,
                        (struct sockaddr *) &client_addr,
                        (socklen_t *) &client_length))
                    == -1) {
            printf("Failed receiving data \n");
            break;
        }
        printf("received %i bytes \n", recv_len);

        printf("received guack: \n");
        for (int i = 0; i<recv_len; i++) {
            printf("byte %i: %i\n", i, *(buf + i));
        }

        int packet_type = identify_packet_type(buf);
        printf("packet type: %i \n", packet_type);

        // FIXME: only proceed if request type

        if ((data_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            printf("couldn't build socket");
            return 1;
        }

        ssize_t sent_bytes;
        if ((sent_bytes = sendto(
                    data_socket,
                    buf,
                    recv_len,
                    0,
                    (struct sockaddr *) &client_addr,
                    (socklen_t) client_length))
                == -1) {
            printf("Problem sending stuff: %i \n", errno);
		}
        printf("Sent %zi bytes \n", sent_bytes);
    }

    close(control_socket);
}
