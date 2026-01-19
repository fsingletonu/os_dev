#include <init/desc.h>

void idt_init()
{
    for (size_t i = 0; i < INT_NUM; i++)
    {
        idt[i].low_offset = (uint16_t)(0x0000);
        idt[i].segment_selector = (uint16_t)(0x0008);
        idt[i].reserved = (uint8_t)(0x00);
        idt[i].TYPE = 0;
        idt[i].S = 0;
        idt[i].DPL = 0;
        idt[i].P = 0;
        idt[i].high_offset = (uint16_t)(0x0000);
    }
    idtr.limit = INT_NUM * 8 - 1;
    idtr.base = (uint32_t)&idt;
}