/*
 * numa.h
 * (C) 2026 Guo Fu
 */
#ifndef _NUMA_H_
#define _NUMA_H_

#include <datatype.h>
#include <init/acpi.h>

typedef struct bootmem_data
{
    uint32_t ards_buffer;
    uint32_t memory_page;
    uint32_t pt_addr;
    uint32_t kernel_mapping_addr;
    uint32_t kernel_load_addr;
    uint32_t current_bump_ptr;
    bool numa_flags;
} _packed bootmem_data_t;
static bootmem_data_t bootmem_data;

enum node_status
{
    N_NORMAL_MEMORY = 0, // 普通内存，个人认为用于UMA架构
    N_NOT_CPU,           // 未被CPU占用
    N_CPU                // 被CPU占用
};

typedef struct mmnode
{
    uint32_t mmnode_id;
    enum node_status status;
    uint64_t persent_pages_num;
    uint64_t spanned_pages_num;
    uint64_t start_pfn;
    addr_t zone;         // 挂载该节点的第一个zone，为了不造成循环依赖，采用通用地址指针
    struct mmnode *next; // 其它的node
} _packed mmnode_t;
static mmnode_t *first_node;

void bootmem_data_init();
void mmnode_init();

#endif
