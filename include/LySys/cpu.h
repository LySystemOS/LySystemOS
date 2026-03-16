#pragma once

#include <LySys/pmm.h>
#include <LySys/paging.h>
#include <LySys/heap.h>
#include <LySys/time.h>
#include <LySys/mbi.h>

extern uint32_t mbi_address;

extern int cpuid_detect();
extern void traps_init();

void cpu_init() {
    uint64_t phys_mbi = (uint64_t)mbi_address;
    if (phys_mbi > KERNEL_VIRTUAL_BASE) {
        phys_mbi -= KERNEL_VIRTUAL_BASE;
    }
    pmm_init(phys_mbi);
    mbi_parse(phys_mbi);
    paging_init();
    time_init();
    traps_init();
    heap_init(HEAP_START_ADDR, HEAP_START_ADDR + HEAP_SIZE);
    cpuid_detect();
    __asm__ volatile("sti");
}