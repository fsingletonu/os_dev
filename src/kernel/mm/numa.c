/*
 * numa.c
 * (C) 2026 Guo Fu
 */
#include <mm/numa.h>

void bootmem_data_init()
{
    uint32_t *ards_buffer_ptr = (uint32_t *)0x7c00;
    uint32_t *memory_page_ptr = (uint32_t *)0x7c04;
    uint32_t *pt_addr_ptr = (uint32_t *)0x7c08;
    uint32_t *kernel_mapping_addr_ptr = (uint32_t *)0x7c0c;
    uint32_t *kernel_load_addr_ptr = (uint32_t *)0x7c10;
    uint32_t *current_bump_ptr = (uint32_t *)0x7c14;
    bootmem_data.ards_buffer = (uint32_t)*ards_buffer_ptr;
    bootmem_data.memory_page = (uint32_t)*memory_page_ptr;
    bootmem_data.pt_addr = (uint32_t)*pt_addr_ptr;
    bootmem_data.kernel_mapping_addr = (uint32_t)*kernel_mapping_addr_ptr;
    bootmem_data.kernel_load_addr = (uint32_t)*kernel_load_addr_ptr;
    bootmem_data.current_bump_ptr = (((uint32_t)*current_bump_ptr) + 4095) & 0xfffff000;
}

void mmnode_init()
{
    bootmem_data_init();
    uint32_t keep = bootmem_data.current_bump_ptr;
    keep = (keep + 4095) & 0xfffff000;
    first_node = (mmnode_t *)keep;
    // 这里说明不是NUMA结构，区域zone指针就放在这一页吧
    if (!seek_acpi_header(SRAT))
    {
        bootmem_data.numa_flags = false;
        bootmem_data.current_bump_ptr += 4096;
        first_node->mmnode_id = 0;
        first_node->status = N_NORMAL_MEMORY;
        first_node->persent_pages_num = bootmem_data.memory_page;
        first_node->spanned_pages_num = 0;
        first_node->start_pfn = 0;
        first_node->zone = (addr_t)(first_node) + sizeof(struct mmnode);
        first_node->next = NULLPTR;
    }
    // 这里就说明是NUMA结构
    else
    {
        bootmem_data.numa_flags = true;
        acpi_sdt_header_t *ptr = seek_acpi_header(SRAT);
    }
}