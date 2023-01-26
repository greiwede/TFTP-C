#include <stdio.h>
#include <stdlib.h>

#include "../Shared/packets.h"

uint16_t inquire_request_type() {
    char requested_mode;
    do {
        fflush(stdin);
        printf("Specify mode [0 for read, 1 for write]: \n");
        requested_mode  = getchar();
    } while (requested_mode != '0' && requested_mode != '1');

    fflush(stdin); // getchar only reads one byte, if user enters more the buffer is not empty

    if (requested_mode == '0') {
        return OPCODE_RRQ;
    } else {
        return OPCODE_WRQ;
    }
}

char * inquire_file_name() {
    // FIXME: redo if invalid file name!
    char * file_name = malloc(MAX_FILE_NAME_LENGTH);
    // FIXME: how to check how many chars in file name?
    printf("Specify file: \n");
    scanf("%100s", file_name);
    return file_name;
}
