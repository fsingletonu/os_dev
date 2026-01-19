#include <lib/panic.h>
static char emes_mem[] = "alloc memory error...\n";

void panic(char *string)
{
    write_string(string, sizeof(string) - 1);
}