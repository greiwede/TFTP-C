#ifndef READ_PACKETS_H_
#define READ_PACKETS_H_

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>

int read_timeout(
        int socket_fd,
        int8_t buf[],
        struct sockaddr_in * addr,
        int expected_block);

int identify_packet_type(uint8_t * frame);

struct request_packet * convert_buf_to_request_packet(uint8_t * buf, ssize_t received_bytes);
#endif
