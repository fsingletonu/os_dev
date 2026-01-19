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

void alloc_pages()
{
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
            // 如果非空就用头插法进行
            if (zone_t.free_list[order].next)
            {
                zone_t.free_list[order].free_count++;
                struct page *keep = zone_t.free_list[order].next;
                struct page *new_blk = (struct page *)PFNTO_TO_PDESCADDR(k_page);
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
}

void bump_allocator()
{
    get_para();
    pfn_init();
    buddy_init();
}
