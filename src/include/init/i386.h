#ifndef _I386_H_
#define _I386_H_

#define magic_bochs() asm volatile("xchg bx,bx\n\t")
#define cli() asm volatile("cli\n\t")
#define sti() asm volatile("sti\n\t")

void _lidt(uint32_t addr);

#endif
