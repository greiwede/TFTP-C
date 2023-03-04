#ifndef RECEIVE_DATA_H_
#define RECEIVE_DATA_H_

#include <arpa/inet.h>
#include <sys/socket.h>

#include "../Packet_Manipulation/packets.h"

#define TIMEOUT_SECONDS 5
#define TIMEOUT_MICRO_SECONDS 0
#define MAX_RETRANSMITS 2

void receive_file(
        struct request_packet * request,
        int * socket,
        struct sockaddr_in * address,
        int address_length);

struct data_packet * receive_packet(
        uint8_t block_number,
        int * socket,
        uint8_t * buf,
        struct sockaddr_in * address,
        int address_length);

int receive_buffer(
        int * socket,
        uint8_t * buf,
        struct sockaddr_in * address,
        int length);

int set_receiving_timeout(int socket_fd);

#endif
