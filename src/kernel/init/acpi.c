/*
 * acpi.c
 * (C) 2026 Guo Fu
 */
#include <init/acpi.h>

RSDPDescriptor *seek_rsdp_addr(addr_t start_addr)
{
    char *keep = (char *)start_addr;
    char *end = (char *)(ACPI_START_ADDR + ACPI_SIZE);
    char rsdp_signature[8] = "RSD PTR ";
    size_t next[8] = {0};
    build_kmp_table_static(rsdp_signature, sizeof(rsdp_signature), next);
    size_t j = 0;
    while (keep != end)
    {
        while (*keep != rsdp_signature[j] && j > 0)
            j = next[j - 1];
        if (*keep == rsdp_signature[j])
        {
            keep++;
            j++;
        }
        else
        {
            keep++;
        }
        if (j == sizeof(rsdp_signature))
        {
            return (RSDPDescriptor *)(keep - j);
        }
    }
    return (RSDPDescriptor *)NULLPTR;
}

void get_rsdp()
{
    rdsp = seek_rsdp_addr((addr_t)ACPI_START_ADDR);
}

bool is_acpi_v1(const RSDPDescriptor *rsdp)
{
    return rsdp->Revision <= 1;
}

bool is_acpi_v2(const RSDPDescriptor *rsdp)
{
    return rsdp->Revision >= 2;
}