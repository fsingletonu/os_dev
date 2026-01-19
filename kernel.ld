ENTRY(_start)

SECTIONS
{
    . = 0xC0000000;

    /* 强制 _start 在 0xC0000000 */
    .text._start : {
        *(.text._start)
    }

    /* 其余内核代码 */
    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(COMMON)
        *(.bss*)
    }
}
