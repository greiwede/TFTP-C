#define STR2(x) #x
#define STR(X) STR2(X)

#include "user_interaction.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "../Shared/Packet_Manipulation/packets.h"

uint16_t inquire_request_type() {
    unsigned char requested_mode;
    int input=-1;

    if (set_stdin_nonblocking() == -1) {
        printf("Error setting stdin nonblocking \n");
        // TODO: handle error
    }
    input = getchar();
    if (set_stdin_blocking() == -1) {
        printf("Error setting stdin blocking \n");
        // TODO: handle error
    }
    while ((input != '\n' && input != EOF) && input != -1) {
        input = getchar();
    };

    do {
        printf("Specify mode [1 for read, 2 for write]: \n");
        requested_mode = getchar();
        printf("You entered the character: %c\n", requested_mode);
        while ((input = getchar()) != '\n' && input != EOF);
    } while (requested_mode != '1' && requested_mode != '2');

    if (requested_mode == '1') {
        return OPCODE_RRQ;
    } else {
        return OPCODE_WRQ;
    }
}

char * inquire_file_name(uint16_t request_type) {
    // XXX: look at possible buffer overflows
    char * file_name = malloc(MAX_FILE_NAME_LENGTH);
    printf("Specify file: \n");
    scanf("%" STR(MAX_FILE_NAME_LENGTH) "s", file_name);
    if (request_type == OPCODE_WRQ) {
        while (access(file_name, F_OK) == -1) {
            printf("Please specify a file that exists: \n");
            scanf("%" STR(MAX_FILE_NAME_LENGTH) "s", file_name);
        }
    }
    return file_name;
}

//FIXME: Read in
char * inquire_mode() {
    return MODE_NETASCII;
}


int set_stdin_nonblocking() {
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    if (flags == -1) {
            return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(STDIN_FILENO, F_SETFL, flags) == -1) {
        return -1;
    }
    return 0;
}


int set_stdin_blocking() {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags == -1) {
            return -1;
        }
    flags &= ~O_NONBLOCK;
    if (fcntl(STDIN_FILENO, F_SETFL, flags) == -1) {
            return -1;
        }
    return 0;
}
