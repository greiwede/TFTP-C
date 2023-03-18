/**
 * \file            write_packets.h
 * \brief           Prepare packets for sending header file
 */
#ifndef WRITE_PACKETS_H_
#define WRITE_PACKETS_H_

#include "packets.h"

packet_meta * build_request_frame(request_packet * information);
packet_meta * build_data_frame(data_packet * information);
packet_meta * build_ack_frame(ack_packet * information);
packet_meta * build_error_frame(error_packet * information);

#endif
