#ifndef _I386_H_
#define _I386_H_

#define cli() asm volatile("cli\n\t")
#define sti() asm volatile("sti\n\t")

#endif
