#ifndef _OSDEV_H_
#define _OSDEV_H_

#include <datatype.h>

#include <chr_drv/console.h>
#include <chr_drv/io.h>
#include <chr_drv/keyboard.h>

#include <init/acpi.h>
#include <init/desc.h>
#include <init/handler.h>
#include <init/i386.h>
#include <init/interrupt.h>
#include <init/trap.h>

#include <lib/math.h>
#include <lib/panic.h>
#include <lib/string.h>

#include <mm/alloc.h>
#include <mm/memory.h>
#include <mm/page.h>
#include <mm/slab.h>

void kernel_init(void);

#endif
