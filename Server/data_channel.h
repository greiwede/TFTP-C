#ifndef DATA_CHANNEL_H_
#define DATA_CHANNEL_H_

#include <arpa/inet.h>

struct request_params {
    uint8_t * buf;
    struct sockaddr_in * client_addr;
    int client_length;
};

void * handle_request(void * request_params);

#endif
