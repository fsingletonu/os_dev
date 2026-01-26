#include <osdev.h>

char message[] = "Hello World!!!\n";

void kernel_init()
{
    cli();
    console_init();
    interrupt_init();
    bump_allocator();
    get_rsdp();
    acpi_fadt_t *m = (acpi_fadt_t *)seek_acpi_header(SSDT);
    while (true)
    {
    }
}