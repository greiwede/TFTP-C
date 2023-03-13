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
    struct socket_meta * control_socket_information = malloc(sizeof(struct socket_meta));
    struct socket_meta * data_socket_information = malloc(sizeof(struct socket_meta));

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("couldn't build socket");
        return 1;
    }

    // FIXME: get server address from command line

    set_receiving_timeout(socket_fd);

    server_control_addr.sin_family = AF_INET;
    server_control_addr.sin_port = htons(TFTP_PORT);
    server_control_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

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

        // TODO:
        // Host A sends a request to host B. Somewhere in the network, the request packet is
        // duplicated, and as a result two acknowledgments are returned to host A, with different
        // TID's chosen on host B in response to the two requests.  When the first response
        // arrives, host A continues the connection.  When the second response to the request
        // arrives, it should be rejected, but there is no reason to terminate the first
        // connection. Therefore, if different TID's are chosen for the two connections on host B
        // and host A checks the source TID's of the messages it receives, the first connection can
        // be maintained while the second is rejected by returning an error packet.

        if (packet->opcode == OPCODE_RRQ) {
            receive_file(packet, data_socket_information);
        } else {
            uint8_t * buf = malloc(sizeof(uint8_t) * ACK_PACKET_LENGTH);
            ssize_t received_bytes;
            if ((received_bytes = receive_buffer(data_socket_information, buf, PACKET_MAX_LENGTH))
                    == -1) {
                printf("Couldn't establish connection to server \n");
            }
            printf("received bytes: %zu \n", received_bytes);
            struct ack_packet * ack = convert_buf_to_ack_packet(buf, received_bytes);
            if (ack != NULL) {
                if (ack->block_no == 0) {
                    send_file(packet, data_socket_information);
                }
            } else {
                struct error_packet * error_packet = convert_buf_to_error_packet(buf, received_bytes);
                if (error_packet == NULL) {
                    printf("Unknown error occured receiving data");
                    free(buf);
                    return -1;
                }
                printf("Error: %i - %s", error_packet->error_code, error_packet->error_message);
                free_error_packet(error_packet);
                free(buf);
                return -1;
            }
            free_ack_packet(ack);
            free(buf);
        }
        // TODO: free here?
        free_request_packet(packet);
        free_packet_meta(packet_meta);
    }
    free(control_socket_information);
}
