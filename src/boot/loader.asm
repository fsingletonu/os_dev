[bits 16]
[org 0x1000]

dw 0x55aa

detect_memory:
    xor ebx,ebx
    mov ax,0
    mov es,ax
    mov di,ards_buffer
    .next:
        mov edx,0x534d4150
        mov eax,0xe820
        mov ecx,20
        int 0x15
        jc error
        cmp eax, 0x534D4150
        jne error
    add di, cx
    inc word [ards_count]
    cmp ebx,0
    jne .next

pre_protected:
    cli

    in al,0x92
    or al,0x02
    out 0x90,al

    lgdt [gdt_ptr]
    lidt [idt_ptr]

    mov eax,cr0
    or eax,0x00000001
    mov cr0,eax

    jmp dword code_selector:protected
        
error:
    mov si,.msg
    call print
    hlt
    jmp $
    .msg db "loading error...",10,13,0

[bits 32]

protected:
    mov ax,data_selector
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    xor eax,eax
    
; because 32-bits adddress calculate the size of memory

    mov ax,[ards_count]
    sub ax,2
    mov bx,20
    mul bx
    add eax,ards_buffer
    mov ebx,[eax]
    mov esi,[eax+8]
    add ebx,esi
    shr ebx,10
    mov [memory_k],ebx

    xor eax,eax

    mov eax,0x000100000
    mov cr3,eax
    
    mov ebx,[memory_k]
    shr ebx,2
    mov [memory_page],ebx

    mov ebx,cr3
    add ebx,4096
    mov [pt_addr],ebx

    mov ecx,[memory_page]
; now ecx keep the count of pde (register one pde after 4 offset byte address)
    shr ecx,10
    mov ebx,cr3
    xor esi,esi

register_pde:
    xor eax,eax
    mov byte [ebx],0b0_0_0_0_0_0_1_1
    mov eax,[pt_addr]
    shr eax,8
    and al,0xf0
    mov byte [ebx+1],al
    shr eax,8
    mov byte [ebx+2],al
    shr eax,8
    mov byte [ebx+3],al
    inc esi
    add ebx,4
    mov eax,[pt_addr]
    add eax,4096
    mov [pt_addr],eax
    cmp esi,ecx
    jne register_pde

    shl ecx,12
    mov eax,[pt_addr]
    sub eax,ecx

    mov [pt_addr],eax

    mov ebx,[pt_addr]
    mov ecx,[memory_page]
    xor esi,esi

register_pte:
    xor eax,eax
    mov byte [ebx],0b0_0_0_0_0_0_1_1
    mov eax,esi
    shl eax,12
    shr eax,8
    and al,0xf0
    mov byte [ebx+1],al
    shr eax,8
    mov byte [ebx+2],al
    shr eax,8
    mov byte [ebx+3],al
    add ebx,4
    inc esi
    cmp esi,ecx
    jne register_pte

; mapping the kernel into high mapping address
    mov eax,[memory_page]
    shl eax,2
    mov ebx,[pt_addr]
    add eax,ebx
    mov [kernel_mapping_addr],eax

    mov ebx,[kernel_size]
    shr ebx,10
    add eax,ebx
    mov [kernel_load_addr],eax

    mov ebx,[kernel_mapping_addr]
    mov ecx,[kernel_size]
    shr ecx,12
    xor esi,esi
register_kernel:
    xor eax,eax
    mov byte [ebx],0b0_0_0_0_0_0_1_1
    mov eax,[kernel_load_addr]
    shr eax,8
    and al,0xf0
    mov byte [ebx+1],al
    shr eax,8
    mov byte [ebx+2],al
    shr eax,8
    mov byte [ebx+3],al
    add ebx,4
    inc esi
    mov eax,[kernel_load_addr]
    add eax,4096
    mov [kernel_load_addr],eax
    cmp ecx,esi
    jne register_kernel

    mov eax,[kernel_load_addr]
    mov ebx,[kernel_size]
    sub eax,ebx
    mov [kernel_load_addr],eax

    mov eax,[pt_addr]

    mov ebx,cr3
    mov eax,768
    shl eax,2
    add ebx,eax
    mov eax,[kernel_mapping_addr]
    mov edi,eax
    mov ecx,256
    xor esi,esi

register_pdt_kernel:
    mov byte [ebx],0b0_0_0_0_0_0_1_1
    shr eax,8
    and al,0xf0
    mov byte [ebx+1],al
    shr eax,8
    mov byte [ebx+2],al
    shr eax,8
    mov byte [ebx+3],al
    add ebx,4
    add edi,4096
    mov eax,edi
    inc esi
    cmp ecx,esi
    jne register_pdt_kernel

; load kernel into memory
    xor edi,edi
    xor ecx,ecx
    xor ebx,ebx
    mov edi,[kernel_load_addr]
    mov ecx,10
    mov bl,240
    call read_disk

; flush TLB and cache
    invlpg [0xc0000000]

; enable page mode
    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax

; pass parameters to the kernel
    mov eax,[memory_page]
    mov dword [0x7c00],eax
    mov eax,[pt_addr]
    mov dword [0x7c04],eax
    mov eax,[kernel_mapping_addr]
    mov dword [0x7c08],eax
    mov eax,[kernel_load_addr]
    mov dword [0x7c0c],eax

; enter kernl
    jmp dword code_selector:0xc0000000

    jmp $

read_disk:
    mov dx,0x1f2
    mov al,bl
    out dx,al

    inc dx
    mov al,cl
    out dx,al

    inc dx
    shr ecx,8
    mov al,cl
    out dx,al

    inc dx
    shr ecx,8
    mov al,cl
    out dx,al

    inc dx
    shr ecx,8
    and cl,0b1111
    mov al,0b1110_0000
    or al,cl
    out dx,al

    inc dx
    mov al,0x20
    out dx,al

    xor ecx,ecx
    mov cl,bl

    .read:
        push cx
        call .waits
        call .reads
        pop cx
        loop .read
    ret

    .waits:
        mov dx,0x1f7
        .check:
            in al,dx
            jmp $+2
            jmp $+2
            jmp $+2
            and al,0b1000_1000
            cmp al,0b0000_1000
            jnz .check
        ret
    
    .reads:
        mov dx,0x1f0
        mov cx,256
        .readw:
            in ax,dx
            jmp $+2
            jmp $+2
            jmp $+2
            mov [edi],ax
            add edi,2
            loop .readw
        ret

print:
    mov ah,0x0e
    .next:
        mov al,[si]
        cmp al,0
        jz .done
        int 0x10
        inc si
        jmp .next
    .done:
        ret

mapping:

    ret

code_selector equ (1<<3) ; flat_mode
data_selector equ (2<<3) ; flat_mode

; 这里的是以4k为粒度，所以GDT中的相关位的单位也是4k
memory_base equ 0
memory_limit equ (1024*1024*1024*4)/(1024*4)-1

; 向GDTR中存储的信息有GDT的大小和基地址，对于GDT来说，所有表项都是连续的，可以在后续的C语言中，采用顺序表的形式去完成GDT和IDT的声明
; 一个段的最大界限为1M，这里是内核1M，不用再另外申请空间，100k的内核主程序，8M可以用256个字节管理，以此类推32M用1k去管理（以页为单位管理）
; 只给内存管理的1k字节，后续扩核，再大改；内存分配池给256k，内存的其它管理需要自己想，没有硬件支持
gdt_ptr:
    dw (gdt_end-gdt_base)-1
    dd gdt_base
gdt_base:
    dd 0,0
gdt_code:
    dw memory_limit & 0xffff
    dw memory_base & 0xffff
    db (memory_base >> 16) & 0xff
    db 0b_1_00_1_1_0_1_0
    db 0b1_1_0_0_0000 | (memory_limit >> 16)
    db (memory_base >> 24) & 0xff
gdt_data:
    dw memory_limit & 0xffff
    dw memory_base & 0xffff
    db (memory_base >> 16) & 0xff
    db 0b_1_00_1_0_0_1_0
    db 0b1_1_0_0_0000 | (memory_limit >> 16)
    db (memory_base >> 24) & 0xff
gdt_end:

idt_ptr:
    dw 256 * 8 - 1
    dd idt_base
idt_base:
    times 256 dw 0, 0x0008, 0x8E00, 0

memory_k:
    dd 0

memory_page:
    dd 0

pt_addr:
    dd 0

kernel_size:
    dd 0x40000000

; high mapping pt
kernel_mapping_addr:
    dd 0

kernel_load_addr:
    dd 0

ards_count:
    dw 0
            
ards_buffer: