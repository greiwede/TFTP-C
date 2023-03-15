#ifndef FILE_MANIPULATION_H_
#define FILE_MANIPULATION_H_

#include <stdint.h>

#define ASCII_NL '\n'
#define ASCII_CR '\x0d'
#define ASCII_LF '\x0a'
#define ASCII_CRLF '\x0d' + '\x0a'
#define ASCII_NUL '\x00'
#define ASCII_CRNUL '\x0d' + '\x00'

// NOTE:
// for netascii mode, each character has 7 bit, one bit is always 0

int buf_to_netascii(uint8_t * data, int data_length, uint8_t * netascii_buf);

int buf_from_netascii(uint8_t * buf, int buf_length, uint8_t * last_char);

int handle_netascii_buf(uint8_t * data, int bytes_read, uint8_t * netascii_buf, uint8_t * excess_queue, int * excess_bytes);

struct error_packet * determine_file_opening_error();

#endif
