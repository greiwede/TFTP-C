#ifndef SEND_DATA_H_
#define SEND_DATA_H_

#include <arpa/inet.h>
#include <sys/socket.h>

#include "../Packet_Manipulation/packets.h"

void send_data(
        struct request_packet * request,
        int * socket,
        struct sockaddr_in * address,
        int address_length);

#endif
