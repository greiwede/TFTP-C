#include "read_packets.h"

#include <sys/socket.h>
#include <stdio.h>

#include "constants.h"

int read_timeout(
        int socket_fd,
        int8_t buf[],
        struct sockaddr_in addr,
        int expected_block) {

    int recv_len;

    // time start

    // receive
    int addr_length = sizeof(addr);
    if ((recv_len = recvfrom(
                    socket_fd,
                    buf,
                    DATA_PACKET_MAX_LENGTH,
                    0,
                    (struct sockaddr *) &addr,
                    (socklen_t *) &addr_length))
                == -1) {
        printf("Failed receiving data \n");
    }

    // time stop
    return recv_len;
}
