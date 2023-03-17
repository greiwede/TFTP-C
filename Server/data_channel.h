#ifndef DATA_CHANNEL_H_
#define DATA_CHANNEL_H_

#include <arpa/inet.h>

struct request_params {
    uint8_t * buf;
    ssize_t buf_length;
    struct sockaddr_in * client_addr;
    int client_length;
};

// void pointers as is designed to be threading
void * handle_request(void * request_params);

#endif
