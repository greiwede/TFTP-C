#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "user_interaction.h"

#include "../Shared/Packet_Manipulation/write_packets.h"
#include "../Shared/Packet_Manipulation/read_packets.h"
#include "../Shared/Data_Flow/receive_data.h"
#include "../Shared/Data_Flow/send_data.h"
#include "../Shared/Packet_Manipulation/packets.h"

int main() {
    struct sockaddr_in server_control_addr;
    int server_control_length = sizeof(server_control_addr);
    struct sockaddr_in server_data_addr;
    int server_data_length = sizeof(server_control_addr);
    int socket_fd;

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't build socket");
        return 1;
    }
    set_receiving_timeout(socket_fd);

    server_control_addr.sin_family = AF_INET;
    server_control_addr.sin_port = htons(TFTP_PORT);
    server_control_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        uint16_t request_type = inquire_request_type();
        char * file_name = inquire_file_name(request_type);

        struct request_packet * packet = build_request_packet(
                request_type,
                file_name,
                MODE_NETASCII);
        struct packet_meta * packet_meta =  build_request_frame(packet);

        ssize_t sent_bytes;
        if ((sent_bytes = sendto(
                    socket_fd,
                    packet_meta->ptr,
                    packet_meta->length,
                    0,
                    (struct sockaddr *) &server_control_addr,
                    (socklen_t) server_control_length))
                == -1) {
            printf("Problem sending stuff: %i \n", errno);
		}

        if (packet->opcode == OPCODE_RRQ) {
            receive_file(packet, &socket_fd, &server_data_addr, server_data_length);
        } else {
            uint8_t * buf = malloc(sizeof(uint8_t) * ACK_PACKET_LENGTH);
            ssize_t received_bytes;
            if ((received_bytes = receive_buffer(
                            &socket_fd,
                            buf,
                            ACK_PACKET_LENGTH,
                            &server_data_addr,
                            server_data_length))
                    == -1) {
                printf("Couldn't establish connection to server \n");
            }
            printf("received bytes: %zu \n", received_bytes);
            struct ack_packet * ack = convert_buf_to_ack_packet(buf, received_bytes);
            if (ack != NULL && ack->block_no == 0) {
                send_file(packet, &socket_fd, &server_data_addr, server_data_length);
            }
            free_ack_packet(ack);
            free(buf);
        }

        // TODO: free here?
        free_request_packet(packet);
        free_packet_meta(packet_meta);
    }
}
