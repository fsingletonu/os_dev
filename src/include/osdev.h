#ifndef _OSDEV_H_
#define _OSDEV_H_

#include <datatype.h>

#include <chr_drv/io.h>
#include <chr_drv/console.h>
#include <chr_drv/keyboard.h>

#include <init/i386.h>
#include <init/interrupt.h>
#include <init/trap.h>

void kernel_init();

#endif
