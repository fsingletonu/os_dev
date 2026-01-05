[org 0x1000]

; 这个是为了保证boot加载的成功
dw 0x55aa

mov si,loading
call print

detect_memory:
    xor ebx,ebx

    mov ax,0
    mov es,ax
    mov di,ards_buffer

    mov edx,0x534d4150

.next:
    mov eax,0xe820
    mov ecx,20

    int 0x15

    jc error

    add di,cx

    inc word [ards_count]

    cmp ebx,0
    jnz .next

    mov si,detecting
    call print

    jmp prepare_protected_mode

prepare_protected_mode:

    cli

    in al,0x92
    or al,0b10
    out 0x92,al

    lgdt [gdt_ptr] ; 内核转移之后 

    mov eax,cr0
    or eax,1
    mov cr0,eax

    jmp dword code_selector:protect_mode

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

loading:
    db "loading Franklin...",10,13,0

detecting:
    db "detecting memory success",10,13,0

error:
    mov si,.msg
    call print
    hlt
    jmp $
    .msg db "loading error...",10,13,0

[bits 32]
; 一页的大小为4k
page_size equ 0x1000

; 内核页目录、页表在内存中的位置，一个项就要占用4k的内存空间，要求是4k对齐的
; 最后的页表项中的高20位才是物理地址，后面的03是为了符合结构要求
system_tde equ 0x8000
system_tpe equ 0x9000
system_loader equ 0xa000

; 应当将实模式下的1M内存全部恒等映射到内核页表中
real_mode_address equ 0x00000
kernel_address equ 0x10000
kernel_addres_va equ 0xc0000000

protect_mode:
    mov ax,data_selector
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax

    mov esp,0x10000

    ; 将内核加载进0x10000的位置
    mov edi,0x10000
    mov ecx,10
    mov bl,240
    call read_disk

    xor eax,eax
    xor ebx,ebx
    xor ecx,ecx
    xor edx,edx

    ; 对内存进行虚拟化的映射
    mov ecx,256
    mov ebx,real_mode_address
    mov edx,real_mode_address

    mov eax,edx
    shr eax,22
    mov dword [system_tde+4*eax],system_loader
    or dword [system_tde+4*eax],0x03

    page_loop_function:
        mov eax,edx
        shr eax,12
        and eax,0x3ff
        and ebx,0x11111000
        or ebx,0x03
        .page_loop:
            mov [system_loader+4*eax],ebx
            add ebx,page_size
            add eax,1
            loop .page_loop

    xor eax,eax
    xor ebx,ebx
    xor ecx,ecx
    xor edx,edx

    mov ecx,256
    mov ebx,kernel_address
    mov edx,kernel_addres_va

    mov eax,edx
    shr eax,22
    mov dword [system_tde+4*eax],system_tpe
    or dword [system_tde+4*eax],0x03

    kernel_loop_function:
        mov eax,edx
        shr eax,12
        and eax,0x3ff
        and ebx,0x11111000
        or ebx,0x03
        .kernel_loop:
            mov [system_tpe+4*eax],ebx
            add ebx,page_size
            add eax,1
            loop .kernel_loop

    invlpg [0xc0000000]
    
    xor eax,eax

    mov eax,cr3
    or eax,system_tde
    mov cr3,eax

    xor eax,eax

    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax

    jmp short flush

    flush:
    jmp code_selector:kernel_addres_va

    ud2

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

ards_count:
    dw 0
            
ards_buffer: