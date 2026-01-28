/*
 * mmzone.h
 * (C) 2026 Guo Fu
 */
#ifndef _MMZONE_H_
#define _MMZONE_H_

#include <datatype.h>
#include <mm/numa.h>

#define MAX_ORDER 11

#define ZONE_TYPE_NORMAL_ON

#define ZONE_TYPE_DMA_SIZE 0
#define ZONE_TYPE_DMA32_SIZE 0
#define ZONE_TYPE_NORMAL_SIZE (bootmem_data.memory_page)

enum zone_types
{
#ifdef ZONE_TYPE_DMA_ON
    ZONE_DMA,
#endif

#ifdef ZONE_TYPE_DMA32_ON
    ZONE_TYPE_DMA32,
#endif

#ifdef ZONE_TYPE_NORMAL_ON
    ZONE_TYPE_NORMAL,
#endif

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
    page_t *pfn_table_ptr;
    struct zone *next;
} _packed zone_t;

typedef struct ards
{
    uint64_t base;
    uint64_t limit;
    uint32_t type;
} _packed ards_t;
ards_t *ards_handler;

void mmzone_init();

#endif
