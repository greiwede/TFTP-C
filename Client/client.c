#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../Shared/read_packets.h"
#include "../Shared/constants.h"

int main() {
    int socket_fd;

    struct sockaddr_in server_control_addr;
    int server_control_length = sizeof(server_control_addr);

    struct sockaddr_in server_data_addr;
    int server_data_length = sizeof(server_control_addr);

    int8_t requested_mode;
    int8_t client_buf[DATA_PACKET_MAX_LENGTH];

    memset(client_buf, '\0', sizeof(client_buf));

    // make socket
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't build socket");
        return 1;
    }

    server_control_addr.sin_family = AF_INET;
    server_control_addr.sin_port = htons(TFTP_PORT);
    server_control_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        do {
            fflush(stdin);
            printf("Specify mode [0 for read, 1 for write]: \n");
            requested_mode  = getchar();
        } while (requested_mode != '0' && requested_mode != '1');

        // getchar only reads one byte, if user enters more the buffer is not empty
        fflush(stdin);
        printf("Specify file: \n");
        scanf("%100s", client_buf);

        ssize_t sent_bytes;
        if ((sent_bytes = sendto(
                    socket_fd,
                    &client_buf,
                    sizeof(client_buf),
                    0,
                    (struct sockaddr *) &server_control_addr,
                    (socklen_t) server_control_length))
                == -1) {
            printf("Problem sending stuff: %i \n", errno);
		}

        ssize_t recv_len;
        if ((recv_len = recvfrom(
                        socket_fd,
                        client_buf,
                        REQUEST_PACKET_LENGTH,
                        0,
                        (struct sockaddr *) &server_data_addr,
                        (socklen_t *) &server_data_length))
                    == -1) {
            printf("Failed receiving data \n");
            break;
        }
        printf("received %zi bytes \n", recv_len);
    }
}
