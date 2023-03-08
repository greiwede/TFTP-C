#ifndef RECEIVE_DATA_H_
#define RECEIVE_DATA_H_

#include <arpa/inet.h>
#include <sys/socket.h>

#include "../Packet_Manipulation/packets.h"
#include "../utils.h"

#define TIMEOUT_SECONDS 5
#define TIMEOUT_MICRO_SECONDS 0
#define MAX_RETRANSMITS 2

void receive_file(
        struct request_packet * request,
        struct socket_meta * socket_information);

struct data_packet * receive_packet(
        uint16_t block_number,
        struct socket_meta * socket_information,
        uint8_t * data_buf);

int receive_buffer(
        struct socket_meta * socket_information,
        uint8_t * buf,
        int buf_length);

int set_receiving_timeout(int socket_fd);

#endif
