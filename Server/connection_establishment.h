#ifndef WRITE_PACKETS_H_
#define WRITE_PACKETS_H_

#include <arpa/inet.h>

int bind_control_socket(int control_socket, in_addr_t addr);

#endif
