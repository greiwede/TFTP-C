#include "data_channel.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdint.h>

#include "../SHARED/read_packets.h"
#include "../SHARED/packets.h"
void * handle_request(void * ptr) {

    packet_type = identify_packet_type(buf);
    printf("packet type: %i \n", packet_type);

    // FIXME: only proceed if request type
    if (packet_type != OPCODE_RRQ || packet_type != OPCODE_WRQ) {
    }

    if ((data_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't build socket");
        return 1;
    }

    if ((sent_bytes = sendto(
                data_socket,
                buf,
                recv_len,
                0,
                (struct sockaddr *) &client_addr,
                (socklen_t) client_length))
            == -1) {
        printf("Problem sending stuff: %i \n", errno);
    }
    printf("Sent %zi bytes \n", sent_bytes);
}
