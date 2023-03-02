#ifndef RECEIVE_DATA_H_
#define RECEIVE_DATA_H_

#include <arpa/inet.h>
#include <sys/socket.h>

int read_timeout(
        int socket_fd,
        int8_t buf[],
        struct sockaddr_in * addr,
        int expected_block);

#endif
