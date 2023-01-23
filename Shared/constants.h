#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <arpa/inet.h>

/*
 * Default port for control channel is 69
 */
#define TFTP_PORT 8800

/*
 *
 * TODO: fix to correct size
 */
#define REQUEST_PACKET_LENGTH 100

/*
 * for data packet max total packet length 520
 * and max 512 of actual data and 8 byte header
 */
#define DATA_PACKET_MAX_LENGTH 516

#define ACK_PACKET_LENGTH 4

/*
 *
 * TODO: fix to correct size
 */
#define ERROR_PACKET_LENGTH 4

#endif
