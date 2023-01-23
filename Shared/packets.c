#include "packets.h"

#include <sys/socket.h>
#include <string.h>

request_packet * build_request_packet(
        int16_t opcode,
        int8_t * filename,
        int filename_length,
        int8_t  * mode,
        int mode_length) {
    struct request_packet packet;
    // TODO: malloc
    packet.opcode = opcode;
    return &packet;
}
