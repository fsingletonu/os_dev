/*
 * math.c
 * (C) 2026 Guo Fu
 */
#include <lib/math.h>

bool is2power(size_t i)
{
    return (i & (i - 1)) == 0;
}

size_t get2power(size_t i)
{
    size_t n = 0;
    while (i > 1)
    {
        i >>= 1;
        n++;
    }
    return n;
}

void build_kmp_table_static(const char *pattern, size_t pattern_len, size_t next[])
{
    next[0] = 0;
    size_t j = 0;
    for (size_t i = 1; i < pattern_len; i++)
    {
        while (j > 0 && pattern[i] != pattern[j])
            j = next[j - 1];
        if (pattern[i] == pattern[j])
            j++;
        next[i] = j;
    }
}

addr_t build_kmp_table(const char *pattern, uint32_t pattern_len)
{
    addr_t table;
    return table;
}

addr_t kmpstrmatch(const char *string, const char *pattern, size_t next[])
{
    return NULLPTR;
}