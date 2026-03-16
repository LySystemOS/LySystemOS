[BITS 64]
global switch_to
global thread_starter
extern ExitTask
extern panic

section .data
    panic_msg db "Thread Leaked!", 0

section .text

switch_to:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    mov [rdi], rsp
    mov rsp, rsi
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

thread_starter:
    iretq

    mov rdi, -1
    call ExitTask

    mov rdi, panic_msg
    call panic
    
    cli
    hlt