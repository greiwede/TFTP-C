/**
 * \file            send_data.h
 * \brief           Send file/packet/buffer header file
 */
#ifndef SEND_DATA_H_
#define SEND_DATA_H_

#include <arpa/inet.h>
#include <sys/socket.h>


#include "../utils.h"
#include "../Packet_Manipulation/packets.h"


void send_file(
        struct request_packet * request,
        struct socket_meta * socket_information);

int send_packet(
        uint8_t * data,
        struct data_packet * data_packet,
        uint16_t block_number,
        struct socket_meta * socket_information,
        uint8_t * ack_buf);

int send_buffer(
        struct socket_meta * socket_information,
        uint8_t * buf,
        int buf_length);

#endif
