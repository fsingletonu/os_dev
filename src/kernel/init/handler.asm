[bits 32]

global _int_0x21

extern kb_handler

_int_0x21:
    pushad
    call kb_handler
    popad
    iret