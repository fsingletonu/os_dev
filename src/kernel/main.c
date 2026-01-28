#include <osdev.h>

char message[] = "Hello World!!!\n";

void kernel_init()
{
    cli();
    console_init();
    interrupt_init();
    rsdp_init();
    pci_device_table_init();
    bump_allocator();
    split(first_node->zone, 10);
    split(first_node->zone, 9);
    split(first_node->zone, 8);
    split(first_node->zone, 7);
    split(first_node->zone, 6);
    split(first_node->zone, 5);
    while (true)
    {
    }
}