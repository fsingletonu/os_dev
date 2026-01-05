ENTRY(_start)

SECTIONS
{
    . = 0xc0000000; /* 指定起始地址 */
    .kernel_init_section : {
        *(.kernel_init_section)
    }
    /* 其他段 */
    .text : AT(0x10000) {
        *(.text)
    }
    .data : { *(.data) }
    .bss : { *(.bss) }
}