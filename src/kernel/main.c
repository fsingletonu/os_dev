#include <osdev.h>

char message[] = "Hello World!!!\n";

void kernel_init()
{
    cli();
    console_init();
    interrupt_init();
    bump_allocator();
    while (true)
    {
    }
}