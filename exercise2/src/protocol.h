#include <stdint.h>
#include <arpa/inet.h>

#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef uint32_t protocol_strlen_t; //type used for the length of the line to be transmitted
const protocol_strlen_t PROTOCOL_MAX_STRLEN = 100000; //Maximum length of one line

protocol_strlen_t to_protocol_byteorder(protocol_strlen_t x) {return htonl(x);}
protocol_strlen_t from_protocol_byteorder(protocol_strlen_t x) {return ntohl(x);}

const unsigned char PROTOCOL_MORE_STRINGS = 1;
const unsigned char PROTOCOL_FINISHED = 2;

#endif //PROTOCOL_H
