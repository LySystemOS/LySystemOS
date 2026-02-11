extern long_mode_start

%define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000

section .multiboot_header
header_start:
    dd 0xe85250d6                
    dd 0                         
    dd header_end - header_start 
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) 

    align 8
    dw 5          
    dw 0          
    dd 20         
    dd 1024       
    dd 768        
    dd 32         

    align 8
    dw 0
    dw 0
    dd 8
header_end:

section .data
global mbi_address
mbi_address: dd 0

section .text
bits 32
global start

start:
    mov [mbi_address - KERNEL_VIRTUAL_BASE], ebx
    
    mov esp, stack_top - KERNEL_VIRTUAL_BASE

    mov eax, p3_table - KERNEL_VIRTUAL_BASE
    or eax, 0b11
    mov [p4_table - KERNEL_VIRTUAL_BASE], eax

    mov [p4_table - KERNEL_VIRTUAL_BASE + 511 * 8], eax

    mov eax, p2_table - KERNEL_VIRTUAL_BASE
    or eax, 0b11
    mov [p3_table - KERNEL_VIRTUAL_BASE], eax
    mov [p3_table - KERNEL_VIRTUAL_BASE + 510 * 8], eax

    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [p2_table - KERNEL_VIRTUAL_BASE + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_p2_table

    mov eax, p4_table - KERNEL_VIRTUAL_BASE
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64.pointer_phys - KERNEL_VIRTUAL_BASE]

    jmp gdt64.code:(start64 - KERNEL_VIRTUAL_BASE)

bits 64
start64:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rax, long_mode_start
    jmp rax

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.data: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41)
.pointer_phys:
    dw $ - gdt64 - 1
    dq gdt64 - KERNEL_VIRTUAL_BASE

section .bss
align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096
stack_bottom:
    resb 4096 * 4
stack_top: