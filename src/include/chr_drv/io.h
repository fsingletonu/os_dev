#ifndef IO_H_
#define IO_H_

#include <datatype.h>

// the first parm is port
extern uint8_t in_byte(uint16_t port);
extern uint16_t in_word(uint16_t port);

// the first parm is port, the second parm is data
extern void out_byte(uint16_t port, uint8_t data);
extern void out_word(uint16_t port, uint16_t data);

#endif
