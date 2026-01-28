/*
 * mmzone.c
 * (C) 2026 Guo Fu
 */
#include <mm/mmzone.h>

void mmzone_init()
{
    if (!bootmem_data.numa_flags)
    {
        zone_t *first_zone = (zone_t *)first_node->zone;
        
    }
}