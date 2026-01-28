/*
 * mmzone.c
 * (C) 2026 Guo Fu
 */
#include <mm/mmzone.h>

addr_t Pfnnum_To_PDescaddr(size_t pfnnum, zone_t *zone)
{
    addr_t PDescaddr = (addr_t)NULLPTR;
    addr_t page_table_base_addr = zone->pfn_table_ptr;
    PDescaddr = (addr_t)((addr_t)page_table_base_addr + sizeof(page_t) * pfnnum);
    return PDescaddr;
}

void mmzone_pfn_init(zone_t *zone)
{
    zone->pfn_table_ptr = (page_t *)bootmem_data.current_bump_ptr;
    page_t *page_ptr = zone->pfn_table_ptr;
    for (size_t i = 0; i < zone->present_pages; i++, page_ptr++)
    {
        page_ptr->head = 0;
        page_ptr->type = PAGE_TYPE_FREE;
        page_ptr->status = PAGE_STATUS_FREE;
        page_ptr->ref_count = 0;
        page_ptr->mmap_count = 0;
        page_ptr->order = 0;
        page_ptr->free_ptr.prev = NULLPTR;
        page_ptr->free_ptr.next = NULLPTR;
    }
    bootmem_data.current_bump_ptr += (sizeof(page_t) * zone->present_pages);
    size_t keep = bootmem_data.current_bump_ptr;
    keep = (keep + 4095) & (~4095);
    bootmem_data.current_bump_ptr = keep;
}

void mmzone_buddy_init(zone_t *zone)
{
    for (size_t i = 0; i < MAX_ORDER; i++)
    {
        zone->free_list[i].free_count = 0;
        zone->free_list[i].order = i;
        zone->free_list[i].size = (1 << i) << 12;
        zone->free_list[i].next = NULLPTR;
    }
    mmzone_pfn_init(zone);
    size_t pages = 0;
    int order = MAX_ORDER - 1;
    page_t *rear_page = (page_t *)NULLPTR;
    if (zone->present_pages > 0)
    {
        while (pages <= zone->present_pages && order != -1)
        {
            DEBUG;
            if ((zone->present_pages - pages) >= (1 << order))
            {
                page_t *new_blk = (page_t *)Pfnnum_To_PDescaddr(pages, zone);
                if (zone->free_list[order].next)
                {
                    zone->free_list[order].free_count++;
                    new_blk->head = 1;
                    new_blk->order = order;
                    new_blk->free_ptr.prev = rear_page;
                    rear_page->free_ptr.next = new_blk;
                    new_blk->free_ptr.next = NULLPTR;
                    rear_page = new_blk;
                    pages += (1 << order);
                }
                else
                {
                    zone->free_list[order].free_count++;
                    new_blk->head = 1;
                    new_blk->order = order;
                    new_blk->free_ptr.prev = NULLPTR;
                    new_blk->free_ptr.next = NULLPTR;
                    rear_page = new_blk;
                    zone->free_list[order].next = new_blk;
                    pages += (1 << order);
                }
            }
            else
            {
                rear_page = (page_t *)NULLPTR;
                order--;
            }
        }
    }
}

void mmzone_init()
{
    if (bootmem_data.numa_count == 1)
    {
        zone_t *zone = (zone_t *)first_node->zone;
        zone->type = ZONE_TYPE_NORMAL;
        zone->phy_start_pfn = 0;
        zone->spanned_pages = bootmem_data.memory_page;
        zone->present_pages = bootmem_data.memory_page;
        zone->next = NULLPTR;
        bootmem_data.current_bump_ptr += sizeof(zone_t);
        size_t keep = bootmem_data.current_bump_ptr;
        keep = (keep + 4095) & (~4095);
        bootmem_data.current_bump_ptr = keep;
        mmzone_buddy_init(zone);
        DEBUG;
    }
    else if (bootmem_data.numa_count > 1)
    {
    }
}
