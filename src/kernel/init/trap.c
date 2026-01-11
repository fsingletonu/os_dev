#include <init/trap.h>

void register_trap(uint16_t trap_num, uint32_t func_addr)
{
    idt[trap_num].low_offset = (uint16_t)(func_addr & 0x0000ffff);
    idt[trap_num].segment_selector = (uint16_t)(0x0008);
    idt[trap_num].reserved = (uint8_t)(0x00);
    idt[trap_num].TYPE = 0xf;
    idt[trap_num].S = 0;
    idt[trap_num].DPL = 0;
    idt[trap_num].P = 1;
    idt[trap_num].high_offset = (uint16_t)((func_addr >> 16) & 0x0000ffff);
}