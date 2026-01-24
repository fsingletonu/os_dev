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