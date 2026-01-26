/*
 * acpi.h
 * (C) 2026 Guo Fu
 */
#ifndef _CPI_H_
#define _CPI_H_

#include <datatype.h>
#include <lib/math.h>

#define BIOS_ACPI 1
// #define UEFI_ACPI 1

#ifdef BIOS_ACPI
#define ACPI_START_ADDR 0xe0000
#define ACPI_SIZE 0x20000
#else
#define ACPI_START_ADDR 0x00000
#endif

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
} _packed RSDPDescriptor;

// 只有这一个静态全局瞬时RSDP
RSDPDescriptor *rdsp;

typedef struct acpi_sdt_header
{
    char signature[4];         // 表签名，如 'RSDT', 'FACP'等
    uint32_t length;           // 表的长度（字节）
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

RSDPDescriptor *seek_rsdp_addr(addr_t start_addr);
void get_rsdp();
bool is_acpi_v1(const RSDPDescriptor *rsdp);
bool is_acpi_v2(const RSDPDescriptor *rsdp);

#endif
