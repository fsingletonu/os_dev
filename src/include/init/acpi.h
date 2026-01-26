/*
 * acpi.h
 * (C) 2026 Guo Fu
 */
#ifndef _CPI_H_
#define _CPI_H_

#include <datatype.h>
#include <lib/math.h>
#include <lib/string.h>

#define BIOS_ACPI 1
// #define UEFI_ACPI 1

#ifdef BIOS_ACPI
#define ACPI_START_ADDR 0xe0000
#define ACPI_SIZE 0x20000
#else
#define ACPI_START_ADDR 0x00000
#endif

// 如果不存在SRAT表，则就是UMA架构
#define RSDP "RSD PTR "
#define RSDT "RSDT"
#define XSDT "XSDT"
#define FACP "FACP"
#define APIC "APIC"
#define DSDT "DSDT"
#define SSDT "SSDT"
#define HPET "HPET"
#define SRAT "SRAT"

#define ACPI_SDT_HEADER_LEN sizeof(struct acpi_sdt_header)

typedef struct RSDPDescriptor
{
    char Signature[8];    // "RSD PTR "
    uint8_t Checksum;     // 校验和
    char OEMID[6];        // OEM ID
    uint8_t Revision;     // ACPI 修订版本/版本号
    uint32_t RsdtAddress; // RSDT 物理地址（ACPI 1.0）
    // ACPI 2.0+ 扩展字段
    uint32_t Length;      // RSDP 结构长度
    uint64_t XsdtAddress; // XSDT 物理地址
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
} _packed RSDPDescriptor_t;

// 只有这一个静态全局瞬时RSDP
RSDPDescriptor_t *rdsp;

typedef struct acpi_sdt_header
{
    char signature[4];         // 表签名，如 'RSDT', 'FACP'等
    uint32_t length;           // 表的长度（包括表头和地址表项，单位：字节）
    uint8_t revision;          // ACPI 修订版本
    uint8_t checksum;          // 整个表的校验和（和应为0）
    char oem_id[6];            // OEM ID
    char oem_table_id[8];      // OEM 表ID
    uint32_t oem_revision;     // OEM 修订版本
    char creator_id[4];        // 创建者ID
    uint32_t creator_revision; // 创建者修订版本
} _packed acpi_sdt_header_t;

typedef struct rsdt
{
    acpi_sdt_header_t header;
    uint32_t table_ptrs[]; // 可变长度数组，包含其他表的32位物理地址
} _packed rsdt_t;

typedef struct xsdt
{
    acpi_sdt_header_t header;
    uint64_t table_ptrs[]; // 可变长度数组，包含其他表的64位物理地址
} _packed xsdt_t;

#define ACPI_FACP_DESC
#ifdef ACPI_FACP_DESC

typedef struct acpi_generic_address
{
    uint8_t address_space_id;    // 地址空间标识符
    uint8_t register_bit_width;  // 寄存器位宽
    uint8_t register_bit_offset; // 寄存器位偏移
    uint8_t access_size;         // 访问大小
    uint64_t address;            // 地址值
} _packed acpi_generic_address_t;

typedef struct acpi_fadt
{
    // 1. 标准 ACPI 表头（36字节）
    acpi_sdt_header_t header; // 签名 "FACP"

    // 2. 硬件寄存器地址（32位物理地址）
    uint32_t firmware_ctrl; // FACS 的物理地址（32位）
    uint32_t dsdt;          // DSDT 的物理地址（32位）

    // 3. 电源管理配置字节
    uint8_t reserved1; // 保留，必须为 0

    // 系统支持的电源管理特性
    uint8_t preferred_pm_profile; // 首选电源管理配置文件
    uint16_t sci_interrupt;       // SCI 系统控制中断向量号
    uint32_t smi_command_port;    // SMI 命令端口
    uint8_t acpi_enable;          // 启用 ACPI 的命令值
    uint8_t acpi_disable;         // 禁用 ACPI 的命令值
    uint8_t s4bios_req;           // 进入 S4 所需的 BIOS 请求
    uint8_t pstate_control;       // 处理器性能状态控制
    uint32_t pm1a_event_block;    // PM1a 事件寄存器块地址
    uint32_t pm1b_event_block;    // PM1b 事件寄存器块地址
    uint32_t pm1a_control_block;  // PM1a 控制寄存器块地址
    uint32_t pm1b_control_block;  // PM1b 控制寄存器块地址
    uint32_t pm2_control_block;   // PM2 控制寄存器块地址
    uint32_t pm_timer_block;      // 电源管理定时器寄存器块地址
    uint32_t gpe0_block;          // 通用事件 0 寄存器块地址
    uint32_t gpe1_block;          // 通用事件 1 寄存器块地址
    uint8_t pm1_event_length;     // PM1 事件寄存器字节长度
    uint8_t pm1_control_length;   // PM1 控制寄存器字节长度
    uint8_t pm2_control_length;   // PM2 控制寄存器字节长度
    uint8_t pm_timer_length;      // PM 定时器寄存器字节长度
    uint8_t gpe0_length;          // GPE0 寄存器字节长度
    uint8_t gpe1_length;          // GPE1 寄存器字节长度
    uint8_t gpe1_base;            // GPE1 块的起始偏移
    uint8_t cstate_control;       // C 状态控制
    uint16_t worst_c2_latency;    // 最坏 C2 状态延迟（微秒）
    uint16_t worst_c3_latency;    // 最坏 C3 状态延迟（微秒）
    uint16_t flush_size;          // 缓存刷新大小
    uint16_t flush_stride;        // 缓存刷新步长
    uint8_t duty_offset;          // 时钟占空比偏移
    uint8_t duty_width;           // 时钟占空比宽度

    // 4. 日期和时间信息
    uint8_t day_alarm;   // RTC CMOS 日闹钟索引
    uint8_t month_alarm; // RTC CMOS 月闹钟索引
    uint8_t century;     // RTC CMOS 世纪索引

    // 5. ACPI 1.0 兼容性字段
    uint16_t boot_flags; // IA-PC 引导架构标志
    uint8_t reserved2;   // 保留，必须为 0
    uint32_t flags;      // 各种功能标志位

    // 6. 复位寄存器
    struct acpi_generic_address reset_register; // 复位寄存器描述
    uint8_t reset_value;                        // 复位寄存器的写入值

    // 7. 扩展字段（ACPI 2.0+）
    uint64_t x_firmware_control; // FACS 的64位物理地址
    uint64_t x_dsdt;             // DSDT 的64位物理地址

    // 8. 扩展的电源管理寄存器（64位地址）
    struct acpi_generic_address x_pm1a_event_block;
    struct acpi_generic_address x_pm1b_event_block;
    struct acpi_generic_address x_pm1a_control_block;
    struct acpi_generic_address x_pm1b_control_block;
    struct acpi_generic_address x_pm2_control_block;
    struct acpi_generic_address x_pm_timer_block;
    struct acpi_generic_address x_gpe0_block;
    struct acpi_generic_address x_gpe1_block;

    // 9. 睡眠控制寄存器（ACPI 5.0+）
    struct acpi_generic_address sleep_control_register;
    struct acpi_generic_address sleep_status_register;

    // 10. 超时值（ACPI 6.0+）
    uint64_t hypervisor_vendor_identity; // 虚拟机监控程序供应商ID

} _packed acpi_fadt_t;

#endif

#define ACPI_APIC_DESC
#ifdef ACPI_APIC_DESC

#define ACPI_MADT_PCAT_COMPAT (1 << 0) // 系统也包含兼容的8259双PIC

typedef struct acpi_madt
{
    acpi_sdt_header_t header;    // 标准表头，签名 "APIC"
    uint32_t local_apic_address; // 本地APIC的物理基地址（通常是0xFEE00000）
    uint32_t flags;              // 标志位
} _packed acpi_madt_t;

typedef struct acpi_subtable_header
{
    uint8_t type;   // 条目类型
    uint8_t length; // 本条目的总长度（包括这个头）
} _packed acpi_subtable_header_t;

#define ACPI_MADT_ENABLED (1 << 0)        // 处理器已启用
#define ACPI_MADT_ONLINE_CAPABLE (1 << 1) // 处理器可在线启用

typedef struct acpi_madt_local_apic
{
    struct acpi_subtable_header header; // type = 0
    uint8_t processor_id;               // ACPI处理器ID
    uint8_t apic_id;                    // APIC ID
    uint32_t flags;                     // 标志位
} _packed acpi_madt_local_apic_t;

typedef struct acpi_madt_io_apic
{
    struct acpi_subtable_header header; // type = 1
    uint8_t io_apic_id;                 // I/O APIC的ID
    uint8_t reserved;                   // 保留
    uint32_t address;                   // I/O APIC的物理地址
    uint32_t global_irq_base;           // 此I/O APIC处理的全局系统中断起始号
} _packed acpi_madt_io_apic_t;

typedef struct acpi_madt_interrupt_override
{
    struct acpi_subtable_header header; // type = 2
    uint8_t bus;                        // 总线类型（0=ISA）
    uint8_t source;                     // 源IRQ号（总线相关）
    uint32_t global_irq;                // 全局系统中断号
    uint16_t flags;                     // MP兼容中断标志
} _packed acpi_madt_interrupt_override_t;

// 常见覆盖示例：将ISA IRQ0（定时器）重映射到GIN 2
// bus = 0, source = 0, global_irq = 2

typedef struct acpi_madt_nmi_source
{
    struct acpi_subtable_header header; // type = 3
    uint16_t flags;                     // 中断标志
    uint32_t global_irq;                // 全局系统中断号
} _packed acpi_madt_nmi_source_t;

typedef struct acpi_madt_local_apic_nmi
{
    struct acpi_subtable_header header; // type = 4
    uint8_t processor_id;               // 处理器ID（0xFF表示所有处理器）
    uint16_t flags;                     // 中断标志
    uint8_t lint;                       // 本地APIC的LINT引脚（0或1）
} _packed acpi_madt_local_apic_nmi_t;

typedef struct acpi_madt_local_apic_override
{
    struct acpi_subtable_header header; // type = 5
    uint16_t reserved;                  // 保留
    uint64_t address;                   // 64位本地APIC物理地址
} _packed acpi_madt_local_apic_override_t;

typedef struct acpi_madt_local_x2apic
{
    struct acpi_subtable_header header; // type = 9
    uint16_t reserved;                  // 保留
    uint32_t x2apic_id;                 // x2APIC ID（32位）
    uint32_t flags;                     // 标志位（同类型0）
    uint32_t acpi_id;                   // ACPI处理器UID
} _packed acpi_madt_local_x2apic_t;

typedef struct acpi_madt_io_apic_nmi
{
    struct acpi_subtable_header header; // type = 10
    uint8_t io_apic_id;                 // I/O APIC ID
    uint16_t flags;                     // 中断标志
    uint8_t global_irq;                 // 全局系统中断号
} _packed acpi_madt_io_apic_nmi_t;

#endif

#define ACPI_SSDT_DESC
#ifdef ACPI_SSDT_DESC

// 如果有AML定义块头部，有的BIOS/UEFI是没有该头部的，SSDT表头直接接AML字节码
// 有无头部是根据签名直接确定的
typedef struct aml_definition_block_header
{
    uint8_t signature[4];     // "DSDT" 或 "SSDT"
    uint32_t length;          // 包括这个头的长度
    uint8_t revision;         // 与主表头一致
    uint8_t checksum;         // 整个定义块的校验和
    uint8_t oemid[6];         // 与主表头一致
    uint8_t oemtableid[8];    // 与主表头一致
    uint32_t oemrevision;     // 与主表头一致
    uint8_t creatorid[4];     // 与主表头一致
    uint32_t creatoreevision; // 与主表头一致
} _packed aml_definition_block_header_t;

#endif

// 应该在真机上是有bug的，因为bochs生成还是按ACPI2.0生成的
RSDPDescriptor_t *seek_rsdp_addr(addr_t start_addr);
void get_rsdp();
bool is_acpi_v1(const RSDPDescriptor_t *rsdp);
bool is_acpi_v2(const RSDPDescriptor_t *rsdp);
acpi_sdt_header_t *seek_acpi_header(char signature[4]);

#endif
