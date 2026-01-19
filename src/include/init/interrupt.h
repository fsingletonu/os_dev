#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <datatype.h>
#include <chr_drv/io.h>
#include <init/i386.h>
#include <init/desc.h>
#include <init/handler.h>

#define PIC_APIC 0 // 0: pic 1:apic

#define HPCP 0x20
#define HPDP 0x21
#define SPCP 0xa0
#define SPDP 0xa1

#define HIMR 0x21
#define SIMR 0xa1

#define HICW1 0x20
#define HICW2 0x21
#define HICW3 0x21
#define HICW4 0x21

#define SICW1 0xa0
#define SICW2 0xa1
#define SICW3 0xa1
#define SICW4 0xa1

#define NON_SPECIFIC_EOI 0x20

void interrupt_init(void);

void register_irq(uint16_t irq_num, uint32_t func_addr);

#endif
