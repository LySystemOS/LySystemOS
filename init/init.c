#include <LySys/LySystem.h>
#include <LySys/timer.h>
#include <LySys/time.h>
#include <LySys/pmm.h>
#include <LySys/paging.h>
#include <LySys/heap.h>
#include <LySys/mbi.h>
#include <LySys/video.h>
#include <LySys/file/ide.h>
#include <LySys/vfs.h>

extern void traps_init();
extern uint32_t mbi_address;
extern char _kernel_start[];
extern char _kernel_end[];

void start_kernel() {
    __asm__ volatile("cli");

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
    fs_init();
    uint64_t *debug_addrm = (uint64_t *)0x10e0; 
    *debug_addrm = 0x525953594C;
    __asm__ volatile("sti");
    while(1) { __asm__ volatile("hlt"); }

    while(1) {
        __asm__ volatile("hlt");
    }
}