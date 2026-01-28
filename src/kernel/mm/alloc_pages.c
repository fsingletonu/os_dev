/*
 * alloc_pages.c
 * (C) 2026 Guo Fu
 */
#include <mm/alloc_pages.h>

void register_page(uint32_t page_num, uint8_t type)
{
}

void bump_allocator()
{
    bootmem_data_init();
    mmnode_init();
    mmzone_init();
}

bool split(zone_t *zone, size_t order)
{
    page_t *page = zone->free_list[order].next;
    size_t count = zone->free_list[order].free_count;
    if (page != NULLPTR && count != 0)
    {
        zone->free_list[order].free_count--;
        page_t *new_blk1 = page;
        page_t *new_blk2 = (page_t *)((addr_t)page + sizeof(page_t) * (1 << (order - 1)));
        new_blk1->head = 1;
        new_blk2->head = 1;
        new_blk1->order = order - 1;
        new_blk2->order = order - 1;
        new_blk1->free_ptr.next = new_blk2;
        new_blk2->free_ptr.prev = new_blk1;
        zone->free_list[order - 1].free_count += 2;
        page_t *old_blk = zone->free_list[order - 1].next;
        zone->free_list[order - 1].next = new_blk1;
        new_blk1->free_ptr.prev = NULLPTR;
        new_blk2->free_ptr.next = old_blk;
        uint32_t m = sizeof(page_t);
        return true;
    }
    else
        return false;
}

bool expand()
{
}

page_t *alloc_page()
{
    page_t *page;
    return page;
}