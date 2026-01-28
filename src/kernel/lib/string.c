/*
 * string.c
 * (C) 2026 Guo Fu
 */
#include <lib/string.h>

int memcmp(const char *str1, const char *str2, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}