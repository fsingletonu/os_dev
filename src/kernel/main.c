#include <osdev.h>

char message[] = "Hello World!!!\n";

void kernel_init()
{
    cli();
    console_init();
    interrupt_init();
    rsdp_init();
    mmnode_init();
    pci_device_table_init();
    while (true)
    {
    }
}