/**
 * \file            client.c
 * \brief           main method of the client
 */

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

int main(int argc, char *argv[]) {

    // server address for requests
    struct sockaddr_in server_control_addr;
    int server_control_length = sizeof(server_control_addr);
    struct socket_meta * control_socket_information = malloc(sizeof(struct socket_meta));

    // server address for data channel
    struct sockaddr_in server_data_addr;
    int server_data_length = sizeof(server_control_addr);
    struct socket_meta * data_socket_information = malloc(sizeof(struct socket_meta));

    // client socket
    int socket_fd;

    // inquire server IP
    in_addr_t addr;
    if (argc >= 2) {
        if (inet_pton(AF_INET, argv[1], &addr) == 1) {
        } else {
            printf("%s is not a valid IPv4 address\n", argv[1]);
            return -1;
        }
    } else {
        addr = inet_addr("127.0.0.1");
        printf("No IP-Adress given, so localhost is being used \n");
    }

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't build socket");
        return 1;
    }

    set_receiving_timeout(socket_fd);

    server_control_addr.sin_family = AF_INET;
    server_control_addr.sin_port = htons(TFTP_PORT);
    server_control_addr.sin_addr.s_addr = addr;

    control_socket_information->socket = &socket_fd;
    control_socket_information->address = &server_control_addr;
    control_socket_information->length = server_control_length;

    data_socket_information->socket = &socket_fd;
    data_socket_information->address = &server_data_addr;
    data_socket_information->length = server_data_length;

    while (1) {
        uint16_t request_type = inquire_request_type();
        char * file_name = inquire_file_name(request_type);
        char * mode = inquire_mode();

        struct request_packet * packet = build_request_packet(
                request_type,
                file_name,
                mode);
        struct packet_meta * packet_meta =  build_request_frame(packet);

        ssize_t sent_bytes;
        if ((sent_bytes = send_buffer(
                    control_socket_information,
                    packet_meta->ptr,
                    packet_meta->length))
                == -1) {
            printf("Problem sending stuff: %i \n", errno);
		}

        if (packet->opcode == OPCODE_RRQ) {
            receive_file(packet, data_socket_information);
        } else { // automatically WRQ
            uint8_t * buf = malloc(sizeof(uint8_t) * PACKET_MAX_LENGTH);
            ssize_t received_bytes;
            // receive ACK #0 -> confirmation that server is ready to receive data
            if ((received_bytes = receive_buffer(data_socket_information, buf, PACKET_MAX_LENGTH))
                    == -1) {
                printf("Couldn't establish connection to server \n");
            }
            struct ack_packet * ack = convert_buf_to_ack_packet(buf, received_bytes);
            if (ack != NULL) {
                if (ack->block_no == 0) {
                    send_file(packet, data_socket_information);
                }
            } else {
                struct error_packet * error_packet = convert_buf_to_error_packet(buf, received_bytes);
                if (error_packet == NULL) {
                    printf("Unknown error occured receiving data \n");
                    free(buf);
                    return -1;
                }
                printf("Error: %i - %s \n", error_packet->error_code, error_packet->error_message);
                free_error_packet(error_packet);
                free(buf);
                return -1;
            }
            free_ack_packet(ack);
            free(buf);
        }
        free_request_packet(packet);
        free_packet_meta(packet_meta);
    }
    free(control_socket_information);
    free(data_socket_information);
}
