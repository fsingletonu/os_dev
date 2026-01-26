#include <mm/memory.h>

void get_ards_addr(void)
{
    uint32_t *ptr = (uint32_t *)0x7c10;
    ards_handler = (ards_t *)*ptr;
}

void get_page_num(void)
{
    uint32_t *ptr = (uint32_t *)0x7c00;
    mss.page_num = *ptr;
}

void get_sys_pdt_addr(void)
{
    uint32_t keep;
    asm volatile("mov %%cr3,%%eax\n\t"
                 "mov %%eax,%0"
                 : "=m"(keep)
                 :
                 : "%eax");
    mss.sys_pdt_addr = keep;
}

void get_sys_high_kernel_pt(void)
{
    uint32_t *ptr = (uint32_t *)0x7c08;
    mss.sys_high_kernel_pt = *ptr;
}

void get_sys_kernel_load_addr(void)
{
    uint32_t *ptr = (uint32_t *)0x7c0c;
    mss.sys_kernel_load_addr = *ptr;
}

void get_para()
{
    get_page_num();
    get_sys_pdt_addr();
    get_sys_high_kernel_pt();
    get_sys_kernel_load_addr();
    get_ards_addr();
}

void pfn_init()
{
    pfn_table_t.base = mss.sys_kernel_load_addr + KERNEL_SIZE * 4096;
    uint32_t k_limit = sizeof(struct page) * mss.page_num;
    k_limit = (k_limit + 4095) & (~4095);
    pfn_table_t.limit = k_limit;
    page *ptr = (page *)pfn_table_t.base;
    for (size_t i = 0; i < mss.page_num; i++, ptr++)
    {
        ptr->head = 0;
        ptr->type = PAGE_TYPE_FREE;
        ptr->status = PAGE_STATUS_FREE;
        ptr->ref_count = 0;
        ptr->mmap_count = 0;
        ptr->free_ptr.prev = NULLPTR;
        ptr->free_ptr.next = NULLPTR;
    }
}

void buddy_init()
{
    zone_t.type = ZONE_TYPE_NORMAL;
    zone_t.phy_start_pfn = 0;
    zone_t.spanned_pages = mss.page_num;
    zone_t.present_pages = mss.page_num;
    for (size_t i = 0; i < MAX_ORDER; i++)
    {
        zone_t.free_list[i].free_count = 0;
        zone_t.free_list[i].order = i;
        zone_t.free_list[i].size = (1 << i) << 12;
        zone_t.free_list[i].next = NULLPTR;
    }
    uint32_t k_page = 0;
    int32_t order = MAX_ORDER - 1;
    while (k_page < mss.page_num && order != -1)
    {
        if ((mss.page_num - k_page) >= (1 << order))
        {
            // 如果非空就用头插法进行，最后链表反转
            if (zone_t.free_list[order].next)
            {
                zone_t.free_list[order].free_count++;
                struct page *keep = zone_t.free_list[order].next;
                struct page *new_blk = (struct page *)PFNTO_TO_PDESCADDR(k_page);
                new_blk->head = 1;
                new_blk->order = order;
                keep->free_ptr.prev = new_blk;
                new_blk->free_ptr.next = keep;
                new_blk->free_ptr.prev = NULLPTR;
                zone_t.free_list[order].next = new_blk;
                k_page += (1 << order);
            }
            else
            {
                zone_t.free_list[order].free_count++;
                struct page *new_blk = (struct page *)PFNTO_TO_PDESCADDR(k_page);
                new_blk->head = 1;
                new_blk->order = order;
                zone_t.free_list[order].next = new_blk;
                new_blk->free_ptr.next = NULLPTR;
                k_page += (1 << order);
            }
        }
        else
        {
            order--;
        }
    }
    for (size_t i = MAX_ORDER - 1; i != -1; i--)
    {
        struct page *ptr = zone_t.free_list[i].next;
        while (ptr && ptr->free_ptr.next)
        {
            ptr = ptr->free_ptr.next;
        }
        zone_t.free_list[i].next = ptr;
    }
    for (size_t i = MAX_ORDER - 1; i != -1; i--)
    {
        struct page *ptr = zone_t.free_list[i].next;
        uint8_t flags = 1;
        while (flags)
        {
            struct page *k1 = ptr->free_ptr.prev;
            struct page *k2 = ptr->free_ptr.next;
            ptr->free_ptr.prev = k2;
            ptr->free_ptr.next = k1;
            if (ptr->free_ptr.next == NULLPTR)
                flags = 0;
            ptr = ptr->free_ptr.next;
        }
    }
}

/*
 *对于大块的拆分，需要对齐，这个只是拆分，不要有任何其它的操作，只对order的块拆分为两个order-1的块
 */
bool expand(free_area free_list_t)
{
    page *page_t = free_list_t.next;
    uint32_t count = free_list_t.free_count;
    if (count != 0)
    {
        uint32_t now_pfn = PDESCADDR_TO_PFNTO(page_t);
        uint32_t order = page_t->order;
        uint32_t new_order = page_t->order - 1;
        uint32_t new_offset = 1 << new_order;
        new_offset = new_offset * sizeof(struct page);
        struct page *new_blk = (struct page *)((uint32_t)page_t + new_offset);
        page_t->order = new_order;
        new_blk->head = 1;
        new_blk->order = new_order;
        zone_t.free_list[order].free_count--;
        zone_t.free_list[order].next = zone_t.free_list[order].next->free_ptr.next;
        zone_t.free_list[order].next->free_ptr.prev = NULLPTR;
        // 都使用头插法
        if (zone_t.free_list[new_order].next)
        {
            struct page *keep = zone_t.free_list[new_order].next;
            zone_t.free_list[new_order].free_count += 2;
            zone_t.free_list[new_order].next = page_t;
            page_t->free_ptr.next = new_blk;
            new_blk->free_ptr.prev = page_t;
            new_blk->free_ptr.next = keep;
            keep->free_ptr.prev = new_blk;
            return true;
        }
        else
        {
            zone_t.free_list[new_order].next = page_t;
            zone_t.free_list[new_order].free_count += 2;
            page_t->free_ptr.next = new_blk;
            new_blk->free_ptr.prev = page_t;
            new_blk->free_ptr.next = NULLPTR;
            return true;
        }
    }
    else
    {
        return false;
    }
}

/*
 *将前1M的内存先分出去，既避免了alloc_pages中的物理地址判断失误，又避免了因硬件端口预留而导致的出错
 *这里不要使用alloc_pages去处理了，有一个单独的以后也不会遇到的问题
 *对于伙伴系统内存的划分，不需要考虑0x0地址，因为页标识符不在0x0
 *没有的情况下，则order加一
 */
void alloc_system()
{
    page page_t;
    page_t.order = MAX_ORDER -1;
    page_t.status = PAGE_STATUS_UNFREE;
    page_t.type = PAGE_TYPE_SYS_CODE_DATA;
    for (size_t i = 0; i < 1; i++)
    {
        alloc_pages(page_t);
    }
}

page* alloc_pages(page page_t)
{
    bool flags = true;
    page* pfn_addr = 0;
    uint32_t order = page_t.order;
    uint32_t k_order = order;
    do
    {
        // 这个是直接有相应大小的块（成功的情况）
        if (zone_t.free_list[k_order].next && k_order == order)
        {
            struct page *keep = zone_t.free_list[k_order].next->free_ptr.next;
            zone_t.free_list[k_order].next->status = PAGE_STATUS_UNFREE;
            zone_t.free_list[k_order].next->type = page_t.type;
            zone_t.free_list[k_order].next->ref_count++;
            pfn_addr = zone_t.free_list[k_order].next;
            zone_t.free_list[k_order].next = keep;
            zone_t.free_list[k_order].free_count--;
            flags = false;
        }
        // 这个说明有比它高阶的块，具体是高几阶，还得看实际执行（成功的情况）
        // 在这里order是需求，k_order是拆分
        else if (zone_t.free_list[k_order].next && k_order != order)
        {
            for (size_t i = k_order; i != order; i--)
            {
                expand(zone_t.free_list[i]);
            }
            struct page *keep = zone_t.free_list[order].next->free_ptr.next;
            zone_t.free_list[order].next->status = PAGE_STATUS_UNFREE;
            zone_t.free_list[order].next->type = page_t.type;
            zone_t.free_list[order].next->ref_count++;
            pfn_addr = zone_t.free_list[order].next;
            zone_t.free_list[order].next = keep;
            zone_t.free_list[order].free_count--;
            flags = false;
        }
        // 这个是没有空闲块的情况
        else if (!zone_t.free_list[k_order].next)
        {
            k_order++;
        }
        /*
         *这个是全找完了，也没有找到连续的伙伴系统管理块，如果有需要的话，则需要置标志位，去使用valloc()
         *这个是唯一的失败出口，剩下的都是成功的情况
         */
        if (k_order >= MAX_ORDER)
        {
            return NULLPTR;
        }
    } while (flags);
    return pfn_addr;
}

void bump_allocator()
{
    get_para();
    pfn_init();
    buddy_init();
    alloc_system();
}
