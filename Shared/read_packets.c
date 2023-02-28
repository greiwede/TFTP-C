#include "read_packets.h"

#include <sys/socket.h>
#include <stdio.h>

#include "constants.h"
#include "packets.h"
#include "utils.h"

int read_timeout(
        int socket_fd,
        uint8_t buf[],
        struct sockaddr_in * addr,
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

int identify_packet_type(uint8_t * frame) {
    uint16_t opcode = ntohs(uint8_buf_to_uint16(frame));
    printf("Opcode: %i \n",opcode);
    if (opcode >= 0 && opcode <= 5) {
        return opcode;
    } else {
        return -1;
    }
}
