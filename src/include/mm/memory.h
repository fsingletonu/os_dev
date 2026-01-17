/*
 * 这里直接摒弃位示图去作为通用物理页分配器的核心结构
 * 采用node-->zone-->buddy的结构去管理内存
 * 块的分配中 块的信息可以提供注册页表的参数
 * 参数的获取还要看bootloader中是怎么指定的
 * 空闲块使用头插法且块内页的指针只有当需要时才被修改 除此之外 只需要修改头页的指针 最后修改zone_t的指针 来完成尾插的等效
 * 伙伴系统的数据隐藏在zone中
 * */
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <datatype.h>

// the count of page
#define KERNEL_SIZE 30
#define PAGE_SIZE 4096
#define PAGE_OFFSET_4K 12
#define MAX_ORDER 11 // buddy system 2^MAX_ORDER * 4k 为一次可分配的最大内存块 4M

#define PFN_TO_FRAME_4K(pfn) (pfn << PAGE_OFFSET_4K)
#define FRAME_TO_PFN_4K(frame) (frame >> PAGE_OFFSET_4K)

#define PFNTO_TO_PDESCADDR(offset) (pfn_table_t.base + sizeof(struct page) * offset)

enum zone_type
{
    ZONE_TYPE_NORMAL = 0
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
    PAGE_TYPE_RESERVE, // handware reserve
    PAGE_TYPE_SYS_BUDDY
};

enum page_status
{
    PAGE_STATUS_UNFREE = 0,
    PAGE_STATUS_FREE
};

typedef struct ards_t
{
    uint64_t base;
    uint64_t limit;
    uint32_t type;
} _packed ards_t;
ards_t *ards_handler;

typedef struct ms_init
{
    uint32_t page_num;
    uint32_t sys_pdt_addr;
    uint32_t sys_high_kernel_pt;
    uint32_t sys_kernel_load_addr;
} ms_init;
ms_init mss;

typedef struct free_head
{
    struct page *prev;
    struct page *next;
} free_head;

typedef struct free_area
{
    uint32_t free_count;
    uint32_t order; // 阶数
    uint32_t size;  // 块的大小
    struct page* next;
} free_area;

typedef struct zone
{
    uint8_t type;
    uint32_t phy_start_pfn;
    uint32_t spanned_pages; // 一共包括的物理页个数
    uint32_t present_pages; // 实际可以被使用的页
    struct free_area free_list[MAX_ORDER];
} zone;
zone zone_t;

typedef struct page
{
    uint8_t head; // 是否是头页 如果头页 则free_area的order字段生效 0 为非头页
    uint32_t order;
    enum page_type type;
    enum page_status status;
    uint32_t ref_count;
    uint32_t mmap_count;
    free_head free_ptr;
} page;

typedef struct pfn_table
{
    uint32_t base;
    uint32_t limit;
} pfn_table;
pfn_table pfn_table_t;

void bump_allocator(void);
uint32_t get_free_page(void);

#endif
