/*
 * pci.c
 * (C) 2026 Guo Fu
 */
#ifndef _PCI_H_
#define _PCI_H_

#include <datatype.h>
#include <chr_drv/io.h>
#include <init/acpi.h>

#define PCI_CONF_ADDR 0xcf8
#define PCI_CONF_DATA 0xcfc

#define CLASS_STORAGE_CONTROLLER 0x01
#define SUBCLASS_IDE 0x01
#define SUBCLASS_RAID 0x04
#define SUBCLASS_SATA 0x06
#define SUBCLASS_NVM 0x08

#define CLASS_NET_CONTROLLER 0x02
#define SUBCLASS_ETHERNET 0x00

#define CLASS_DISPALY_CONTORLLER 0x03

#define CLASS_MULTIMEDIA 0x04

#define CLASS_MEMORY_CONTROLLER 0x05

#define CLASS_BRIAGE 0x06
#define SUBCLASS_BRIAGE 0x04

#define CLASS_SERIALBUS 0x0c
#define SUBCLASS_USB 0x03
#define SUBCLASS_SMBUS 0x05

typedef struct pci_configSpace
{
    // 0x00 ~ 0x0F 标准头部
    uint16_t vendor_id;      // 0x00
    uint16_t device_id;      // 0x02
    uint16_t command;        // 0x04
    uint16_t status;         // 0x06
    uint8_t revision_id;     // 0x08
    uint8_t prog_if;         // 0x09
    uint8_t subclass;        // 0x0A
    uint8_t class;           // 0x0B
    uint8_t cache_line_size; // 0x0C
    uint8_t latency_timer;   // 0x0D
    uint8_t header_type;     // 0x0E
    uint8_t bist;            // 0x0F

    // 0x10 ~ 0x3F 标准头扩展
    uint32_t bar[6];                // 0x10 ~ 0x27 (BAR0~BAR5)
    uint32_t cardbus_cis_ptr;       // 0x28
    uint16_t subsystem_vendor_id;   // 0x2C
    uint16_t subsystem_id;          // 0x2E
    uint32_t expansion_rom_base;    // 0x30
    uint8_t capabilities_ptr;       // 0x34
    uint8_t reserved1[3];           // 0x35 ~ 0x37
    uint32_t reserved2;             // 0x38 ~ 0x3B
    uint8_t interrupt_line;         // 0x3C
    uint8_t interrupt_pin;          // 0x3D
    uint16_t min_grant_max_latency; // 0x3E ~ 0x3F

    // 0x40 ~ 0xFF 设备扩展区（设备自定义 / 保留）
    uint8_t device_specific[192]; // 0x40 ~ 0xFF
} _packed pci_configSpace_t;

typedef struct pci_device_id
{
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class;
    uint8_t subclass;
} _packed pci_device_id_t;

uint32_t PciConfigReadDword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_device_table_init();

#endif
