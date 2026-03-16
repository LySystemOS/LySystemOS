#ifndef PMM_H
#define PMM_H

#include <LySys/types.h>

extern uint32_t total_blocks;
extern uint32_t used_blocks;

#define PAGE_SIZE 4096
#define BLOCKS_PER_BYTE 8

#define MAX_BLOCKS 65536 
#define BITMAP_SIZE (MAX_BLOCKS / BLOCKS_PER_BYTE)

extern uint8_t pmm_bitmap[BITMAP_SIZE];

void pmm_init(uintptr_t mbi_addr);
void* pmm_alloc_block();
void pmm_free_block(void* ptr);

void pmm_mark_free(uintptr_t phys_addr);
void pmm_mark_used(uintptr_t phys_addr);

#define SET_BIT(i)   (pmm_bitmap[(i) / 8] |=  (1 << ((i) % 8)))
#define CLEAR_BIT(i) (pmm_bitmap[(i) / 8] &= ~(1 << ((i) % 8)))
#define TEST_BIT(i)  (pmm_bitmap[(i) / 8] &   (1 << ((i) % 8)))

#endif