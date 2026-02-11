#ifndef HEAP_H
#define HEAP_H

#include <LySys/types.h>

#define HEAP_START_ADDR 0xFFFFFFFFD0000000
#define HEAP_SIZE (10 * 1024 * 1024)
#define HEAP_MAGIC 0x12345678

typedef struct heap_header {
    uint32_t magic;
    uint8_t is_free;
    uint64_t size;
    struct heap_header *next;
} heap_header_t;

void heap_init(uint64_t start_addr, uint64_t end_addr);
void* kmalloc(uint64_t size);
void kfree(void* ptr);

#endif