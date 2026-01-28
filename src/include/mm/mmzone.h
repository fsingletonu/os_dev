/*
 * mmzone.h
 * (C) 2026 Guo Fu
 */
#ifndef _MMZONE_H_
#define _MMZONE_H_

#include <datatype.h>
#include <mm/numa.h>

#define MAX_ORDER 11

enum zone_types
{
    ZONE_DMA = 0,
    ZONE_DMA32,
    ZONE_NORMAL
};

enum page_type
{
    PAGE_TYPE_SYS_PDE = 0,
    PAGE_TYPE_SYS_PTE,
    PAGE_TYPE_USR_PDE,
    PAGE_TYPE_USR_PTE,
    PAGE_TYPE_SYS_DATA,
    PAGE_TYPE_SYS_CODE,
    PAGE_TYPE_SYS_PDESC,
    PAGE_TYPE_SYS_BITMAP,
    PAGE_TYPE_USR_DATA,
    PAGE_TYPE_USR_CODE,
    PAGE_TYPE_FREE,
    PAGE_TYPE_RESERVE,
    PAGE_TYPE_SYS_BUDDY,
    PAGE_TYPE_SYS_CODE_DATA
};

enum page_status
{
    PAGE_STATUS_UNFREE = 0,
    PAGE_STATUS_FREE
};

typedef struct free_head
{
    struct page *prev;
    struct page *next;
} _packed free_head_t;

typedef struct page
{
    uint8_t head; // 是否是头页 如果头页 则free_area的order字段生效 0 为非头页
    uint32_t order;
    enum page_type type;
    enum page_status status;
    uint32_t ref_count;
    uint32_t mmap_count;
    free_head_t free_ptr;
} _packed page_t;

typedef struct free_area
{
    uint32_t free_count;
    uint32_t order; // 阶数
    uint32_t size;  // 块的大小
    page_t *next;
} _packed free_area_t;

typedef struct zone
{
    uint8_t type;
    uint32_t phy_start_pfn;
    uint32_t spanned_pages; // 一共包括的物理页个数
    uint32_t present_pages; // 实际可以被使用的页
    free_area_t free_list[MAX_ORDER];
    struct zone *next;
} _packed zone_t;

typedef struct pfn_table
{
    uint32_t base;
    uint32_t limit;
} _packed pfn_table_t;
static pfn_table_t pfn_table;

typedef struct ards
{
    uint64_t base;
    uint64_t limit;
    uint32_t type;
} _packed ards_t;
ards_t *ards_handler;

void mmzone_init();

#endif
