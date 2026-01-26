#include <osdev.h>

char message[] = "Hello World!!!\n";
char string[] = "aabaaf\n";
size_t next[6];

void kernel_init()
{
    cli();
    console_init();
    interrupt_init();
    bump_allocator();
    build_kmp_table_static(string, sizeof(string) - 1, next);
    get_rsdp();
    while (true)
    {
    }
}