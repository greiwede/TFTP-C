#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../Shared/Packet_Manipulation/packets.h"

int bind_control_socket(int control_socket) {
    struct sockaddr_in server_addr;

    // bind socket to port and ip
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TFTP_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(control_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        printf("Couldn't bind to the port \n");
        close(control_socket);
        return -1;
    }

    return 0;
}
