/*
 * math.h
 * (C) 2026 Guo Fu
 */
#ifndef _MATH_H_
#define _MATH_H_

#include <datatype.h>

bool is2power(size_t i);
size_t get2power(size_t i);

void build_kmp_table_static(const char *pattern, size_t pattern_len, size_t next[]);

// 还未完全实现malloc()，所以先不实现
addr_t build_kmp_table(const char *pattern, uint32_t pattern_len);
addr_t mem_cmp(const char *string, const char *pattern, size_t next[]);

#endif
