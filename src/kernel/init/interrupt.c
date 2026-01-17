#include <init/interrupt.h>

void pic_init()
{
    out_byte(HIMR, 0xff);
    out_byte(SIMR, 0xff);

    out_byte(HICW1, 0x11);
    out_byte(HICW2, 0x20);
    out_byte(HICW3, 0x04);
    out_byte(HICW4, 0x01);

    out_byte(SICW1, 0x11);
    out_byte(SICW2, 0x28);
    out_byte(SICW3, 0x02);
    out_byte(SICW4, 0x01);

    out_byte(HIMR, 0xfd);
    out_byte(SIMR, 0xff);
}

void apic_init()
{
}

void register_irq(uint16_t irq_num, uint32_t func_addr)
{
    idt[irq_num].low_offset = (uint16_t)(func_addr & 0x0000ffff);
    idt[irq_num].segment_selector = (uint16_t)(0x0008);
    idt[irq_num].reserved = (uint8_t)(0x00);
    idt[irq_num].TYPE = 0xe;
    idt[irq_num].S = 0;
    idt[irq_num].DPL = 0;
    idt[irq_num].P = 1;
    idt[irq_num].high_offset = (uint16_t)((func_addr >> 16) & 0x0000ffff);
}

void interrupt_init()
{
    idtr.limit = 0x7ff;
    idtr.base = (uint32_t)(&idt);
    _lidt((uint32_t)(&idtr));
    if (PIC_APIC)
    {
        apic_init();
    }
    else
    {
        pic_init();
    }
}