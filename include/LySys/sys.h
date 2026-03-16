#pragma once

#include <LySys/errno.h>
#include <LySys/types.h>

#define stdint      0
#define stdout      1
#define stderr      2

#define SYS_write   0
#define SYS_exit    60

void syscall_handler(uint64_t rax, uint64_t rdx, uint64_t rsi, uint64_t rdi);
uint64_t sys_write(uint64_t rdx, uint64_t rsi, uint64_t rdi);
void sys_exit(uint64_t rdx);