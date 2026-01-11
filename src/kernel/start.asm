[bits 32]

section .text._start
extern kernel_init

global _start

_start:
    cli
    call kernel_init
    jmp $