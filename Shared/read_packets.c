#include "read_packets.h"

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>

#include "packets.h"
#include "utils.h"

int read_timeout(
        int socket_fd,
        int8_t buf[],
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


struct request_packet * convert_buf_to_request_packet(uint8_t * buf, ssize_t received_bytes) {
    int packet_type;
    struct request_packet * request;
    int offset;
    int file_name_length;

    if (received_bytes < 6) {
        return NULL;
    }
    packet_type = identify_packet_type(buf);
    if (packet_type != 1 && packet_type != 2) {
        return NULL;
    }

    offset = 0;
    request = malloc(sizeof(struct request_packet));
    request->opcode = packet_type;
    offset += OPCODE_LENGTH;
    char * strings = (char *) buf;
    char * first_string = strtok(strings + offset, "0");
    request->file_name = first_string;
    offset += strlen(first_string) + ZERO_BYTE_LENGTH;
    char * second_string = strtok(strings + offset, "0");
    request->mode = second_string;
    return request;
}
