#ifndef USER_H
#define USER_H

#include <LySys/types.h>
#include <LySys/heap.h>

struct tss_entry {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
} __attribute__((packed));

extern struct tss_entry kernel_tss;

static inline void enter_user_mode(uint64_t entry_point, uint64_t user_stack) {
    void* k_stack = kmalloc(4096) + 4096;
    kernel_tss.rsp0 = (uint64_t)k_stack;

    __asm__ volatile (
        "cli\n"
        "mov $0x23, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "pushq $0x23\n"
        "pushq %0\n"
        "pushfq\n"
        "popq %%rax\n"
        "orq $0x200, %%rax\n"
        "pushq %%rax\n"
        "pushq $0x1B\n"
        "pushq %1\n"
        "iretq\n"
        :
        : "r" (user_stack), "r" (entry_point)
        : "rax", "memory"
    );
}

#endif