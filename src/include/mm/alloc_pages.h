/*
 * alloc_pages.h
 * (C) 2026 Guo Fu
 */
#ifndef _PAGE_H_
#define _PAGE_H_

#include <datatype.h>
#include <mm/mmzone.h>

#define ALLOC_UNFULL 0x00
#define ALLOC_FULL 0x01

#define PAGE_4K 1
#ifdef PAGE_4K
#define PAGE_SIZE 0x1000
#else
#define PAGE_SIZE 0x200000
#endif

// 这两个数据结构当偏移量工具用
typedef struct pde
{
    uint8_t P : 1;                 // 存在位 (Present)
    uint8_t R_W : 1;               // 读写权限 (Read/Write)
    uint8_t U_S : 1;               // 用户/超级用户权限 (User/Supervisor)
    uint8_t PWT : 1;               // 写透 (Write-Through)
    uint8_t PCD : 1;               // 缓存禁用 (Cache Disable)
    uint8_t A : 1;                 // 访问位 (Accessed)
    uint8_t D : 1;                 // 赃位（仅对大页PDE有效）
    uint8_t PS : 1;                // 页大小 (Page Size)
    uint8_t G : 1;                 // 全局位（Global，仅对大页PDE有效）
    uint8_t AVL : 3;               // 可供操作系统使用
    uint32_t page_table_base : 20; // 页表基地址 (Page Table Base Address)
} _packed pde;                     // 页目录项

typedef struct pte
{
    uint8_t P : 1;                 // 存在位
    uint8_t R_W : 1;               // 读写权限
    uint8_t U_S : 1;               // 用户/超级用户
    uint8_t PWT : 1;               // 写透
    uint8_t PCD : 1;               // 缓存禁用
    uint8_t A : 1;                 // 访问位
    uint8_t D : 1;                 // 脏位
    uint8_t PS : 1;                // 页大小
    uint8_t G : 1;                 // 全局页
    uint8_t reserve : 3;           // 保留位
    uint32_t page_frame_base : 20; // 物理页框基地址
} _packed pte;                     // 这个数据结构为四个字节，页表项

void register_page(uint32_t page_num, uint8_t type);

#endif
