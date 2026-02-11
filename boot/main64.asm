global long_mode_start
extern start_kernel

bits 64

section .text
long_mode_start:
    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    lidt [idtr]

    call start_kernel

    cli
.halt:
    hlt
    jmp .halt

section .data
align 16
global idt_table
idt_table:
    times 512 dq 0

idtr:
    dw 4095
    dq idt_table