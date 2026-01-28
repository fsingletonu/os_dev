/*
 * acpi.c
 * (C) 2026 Guo Fu
 */
#include <init/acpi.h>

RSDPDescriptor_t *seek_rsdp_addr(addr_t start_addr)
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
            return (RSDPDescriptor_t *)(keep - j);
        }
    }
    return (RSDPDescriptor_t *)NULLPTR;
}

void rsdp_init()
{
    rdsp = seek_rsdp_addr((addr_t)ACPI_START_ADDR);
}

bool is_acpi_v1(const RSDPDescriptor_t *rsdp)
{
    return rsdp->Revision == 0;
}

bool is_acpi_v2(const RSDPDescriptor_t *rsdp)
{
    return rsdp->Revision >= 2;
}

int judge_acpi_type(char *str1, char *str2)
{
    if (memcmp(str1, str2, 4))
        return true;
    return false;
}

acpi_sdt_header_t *seek_acpi_header(char signature[4])
{
    // 如果是v1.0，就选择4字节解析
    if (is_acpi_v1(rdsp))
    {
        acpi_sdt_header_t *rsdt = (acpi_sdt_header_t *)rdsp->RsdtAddress;
        size_t entry_num = ((size_t)rsdt->length - ACPI_SDT_HEADER_LEN) >> 2;
        addr_t entry = (addr_t)rsdt + ACPI_SDT_HEADER_LEN;
        for (size_t i = 0; i < entry_num; i++)
        {
            addr_t acpi_ptr = (addr_t)(*((uint32_t *)(entry + i * 4)));
            acpi_sdt_header_t *acpi = (acpi_sdt_header_t *)acpi_ptr;
            if(judge_acpi_type(acpi->signature, signature))
            {
                return acpi;
            }
        }
        return NULLPTR;
    }
    // 如果是v2.0，就选择8字节解析，这里还没法写，因为GCC -m32
    if (is_acpi_v2(rdsp))
    {
    }
}