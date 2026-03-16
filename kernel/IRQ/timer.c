#include <LySys/LySystem.h>
#include <LySys/sched.h>
#include <LySys/types.h>

uint64_t ticks = 0;

uint64_t get_uptime() {
    return ticks / 100;
}

void msleep(uint32_t ms) {
    if (ms == 0) return;

    __asm__ volatile("cli");
    struct Task* t = &TaskTable[CurrentTaskPID];
    t->msleep = ms / 10;
    t->state = TASK_SLEEPING;
    __asm__ volatile("sti");

    Schedule();
}

void sleep(uint32_t s) {
    msleep(s * 1000);
}