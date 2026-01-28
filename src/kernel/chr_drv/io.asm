[bits 32]
global in_byte
global in_word
global in_dword
global out_byte
global out_word
global out_dword

in_byte:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    in al,dx
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

in_word:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    in ax,dx
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

in_dword:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    in eax,dx
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

out_byte:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,al
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

out_word:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,ax
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

out_dword:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,eax
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret