#include <osdev.h>

char message[] = "Hello World!!!\n";

void kernel_init()
{
    cli();
    console_init();
    write_string(message, sizeof(message) - 1);
    interrupt_init();
    register_irq(0x21,(uint32_t)&_int_0x21);
    sti();
    while (true)
    {
    }
}