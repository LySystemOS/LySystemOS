#ifndef PAGING_H
#define PAGING_H

#include <LySys/types.h>

#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITE   (1ULL << 1)
#define PAGE_PCD     (1 << 4)
#define PAGE_USER    (1ULL << 2)

void paging_init(void);
void map_page(uint64_t virtual, uint64_t physical, uint64_t flags);
void map_page_ext(uint64_t virtual, uint64_t physical, uint64_t flags, uint64_t pml4_phys);
uint64_t safe_pmm_alloc(void);

#endif