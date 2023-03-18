/**
 * \file            read_packets.h
 * \brief           Handle incoming packets header */
#ifndef READ_PACKETS_H_
#define READ_PACKETS_H_

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>

int identify_packet_type(uint8_t * frame);

struct request_packet * convert_buf_to_request_packet(uint8_t * buf, ssize_t received_bytes);
struct data_packet * convert_buf_to_data_packet(uint8_t * buf, ssize_t received_bytes);
struct ack_packet * convert_buf_to_ack_packet(uint8_t * buf, ssize_t received_bytes);
struct error_packet * convert_buf_to_error_packet(uint8_t * buf, ssize_t received_bytes);

#endif
