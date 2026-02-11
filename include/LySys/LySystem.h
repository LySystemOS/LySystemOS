#pragma once

#define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000

// Visual Studio Fix
#ifdef __INTELLISENSE__
    #undef __asm__
    #define __asm__(...) 
    #undef volatile
    #define volatile 
    #define __asm__volatile__(...)
    #define __attribute__(x)
    #define __extension__
    #define __restrict
    #define __inline inline
#endif

int putchar(int c);
int printk(const char *fmt, ...);
