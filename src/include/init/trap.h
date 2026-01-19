#ifndef _TRAP_H_
#define _TRAP_H_

#include <datatype.h>
#include <init/desc.h>
#include <init/handler.h>

void register_trap(uint16_t trap_num, uint32_t func_addr);

#endif
