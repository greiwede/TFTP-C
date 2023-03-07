#ifndef USER_INTERACTION_H_
#define USER_INTERACTION_H_

#include <stdlib.h>
#include <stdint.h>

uint16_t inquire_request_type();
char * inquire_file_name(uint16_t request_type);
int set_stdin_nonblocking();
int set_stdin_blocking();

#endif
