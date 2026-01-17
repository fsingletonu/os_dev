#ifndef _DESC_H_
#define _DESC_H_

#include <datatype.h>

#define INT_NUM 256

typedef struct gdt_ptr32
{
    uint16_t limit;
    uint32_t base;
} _packed gdt_ptr32;
gdt_ptr32 gdtr;

typedef struct gate_gdt32
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t type : 4;
    uint8_t seg : 1;
    uint8_t DPL : 2;
    uint8_t present : 1;
    uint8_t high_seg : 4;
    uint8_t available : 1;
    uint8_t long_mode : 1;
    uint8_t protected_or_real : 1;
    uint8_t granularity : 1;
    uint8_t base_high;
} _packed gate_gdt32;

typedef struct idt_ptr32
{
    uint16_t limit;
    uint32_t base;
} _packed idt_ptr32;
idt_ptr32 idtr;

typedef struct gate_idt32
{
    uint16_t low_offset;       // 中断入口函数的低16位
    uint16_t segment_selector; // 段选择子，这里的段选择子必须也是在GDT中声明了的段，否则会触发保护模式的禁忌，直接宕机或者重启。
    uint8_t reserved;          // 保留位，全部都是 0
    uint8_t TYPE : 4;          // 0x5：任务门 0x6：16位中断门 0x7：16位陷阱门 0xe：32位中断门 0xf：32位陷阱门
    uint8_t S : 1;             // 0：系统段描述符 1：普通段描述符
    uint8_t DPL : 2;           // 这2位是为了告知保护模式的CPU，此程序段的权限
    uint8_t P : 1;             // 有效位
    uint16_t high_offset;      // 中断入口函数的高16位
} _packed gate_idt32;          // 32位模式下的中断描述符结构

gate_idt32 idt[INT_NUM];

void idt_init(void);

#endif
