#include <LySys/LySystem.h>
#include <LySys/errno.h>
#include <LySys/timer.h>
#include <LySys/time.h>
#include <LySys/pmm.h>
#include <LySys/paging.h>
#include <LySys/heap.h>
#include <LySys/mbi.h>
#include <LySys/video.h>
#include <LySys/file/ide.h>
#include <LySys/vfs.h>
#include <LySys/sched.h>

extern void traps_init();
extern uint32_t mbi_address;
extern char _kernel_start[];
extern char _kernel_end[];

void kernelthread1() {
    while (1);
    ExitTask(0);
}

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
    if (fs_init() != ESUCCESS) {
        panic("FS Mount Error!");
    }
    sched_init();
    CreateTask("task1", kernelthread1, 0);
    while(1) {
    }
}