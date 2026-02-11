#include <LySys/LySystem.h>
#include <LySys/types.h>

uint64_t ticks = 0;

uint64_t get_uptime() {
    return ticks / 100;
}

void msleep(uint32_t ms) {
    uint64_t start_ticks = ticks;
    uint64_t wait_ticks = ms / 10; 
    
    while (ticks < start_ticks + wait_ticks) {
        __asm__ volatile("hlt"); 
    }
}

void sleep(uint32_t s) {
    msleep(s * 1000);
}