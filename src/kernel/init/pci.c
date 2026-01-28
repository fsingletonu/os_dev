/*
 * pci.c
 * (C) 2026 Guo Fu
 */
#include <init/pci.h>

uint32_t PciConfigReadDword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint32_t address = 0;
    uint32_t read_dword = 0;
    uint32_t kbus = (uint32_t)bus;
    uint32_t kdevice = (uint32_t)device;
    uint32_t kfunction = (uint32_t)function;
    address = (uint32_t)((kbus << 16) | (kdevice << 11) | (kfunction << 8) | ((uint32_t)offset & 0xfc) | ((uint32_t)0x80000000));
    out_dword(PCI_CONF_ADDR, address);
    read_dword = in_dword(PCI_CONF_DATA);
    return read_dword;
}

void pci_scan_bus(uint8_t bus)
{
    for (size_t j = 0; j < 32; j++)
    {
        if ((uint16_t)PciConfigReadDword(bus, j, 0, 0x00) == 0xffff)
            continue;

        // 有设备，则至少有一个功能号
        uint32_t k1 = PciConfigReadDword(bus, j, 0, 0x0c);
        bool multi_flags = ((k1 >> 16) & 0x80) != 0;
        if (multi_flags)
        {
            for (size_t k = 0; k < 8; k++)
            {
                if ((uint16_t)PciConfigReadDword(bus, j, k, 0x00) == 0xffff)
                    continue;

                // 开始解析PCI设备表
                
                uint32_t k3 = PciConfigReadDword(bus, j, k, 0x08);
                uint16_t class = (uint16_t)(k3 >> 16);
                uint8_t next_bus = (uint8_t)(PciConfigReadDword(bus, j, k, 0x18) >> 8);
                if (class == ((CLASS_BRIAGE << 8) | SUBCLASS_BRIAGE) && next_bus != 0)
                {
                    pci_scan_bus(next_bus);
                }
            }
        }
        else
        {
            // 开始解析PCI设备表
        }
    }
}

void pci_device_table_init()
{
    // 如果没有MCFG表，就按传统PCI解析，用递归方式遍历PCI树
    if (!seek_acpi_header(MCFG))
    {
        pci_scan_bus(0);
    }
}