#include <LySys/pmm.h>
#include <LySys/LySystem.h>
#include <LySys/mbi.h>

extern uint8_t _kernel_start[];
extern uint8_t _kernel_end[];

#define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000

#define V2P(x) ((uintptr_t)(x) - KERNEL_VIRTUAL_BASE)

#define P2V(x) ((uintptr_t)(x) + KERNEL_VIRTUAL_BASE)

uint8_t pmm_bitmap[BITMAP_SIZE];
uint32_t total_blocks = MAX_BLOCKS;
uint32_t used_blocks = MAX_BLOCKS;

void pmm_mark_used(uintptr_t phys_addr) {
    uint32_t block = phys_addr / PAGE_SIZE;
    if (block >= total_blocks) return; 

    if (!(pmm_bitmap[block / 8] & (1 << (block % 8)))) {
        pmm_bitmap[block / 8] |= (1 << (block % 8));
        used_blocks++;
    }
}

void pmm_mark_free(uintptr_t phys_addr) {
    uint32_t block = phys_addr / PAGE_SIZE;
    if (block >= total_blocks) return; 

    if (pmm_bitmap[block / 8] & (1 << (block % 8))) {
        pmm_bitmap[block / 8] &= ~(1 << (block % 8));
        used_blocks--;
    }
}

void pmm_init(uintptr_t mbi_phys_addr) {
    for (int i = 0; i < BITMAP_SIZE; i++) pmm_bitmap[i] = 0xFF;
    used_blocks = total_blocks;

    uintptr_t mbi_virt_addr = P2V(mbi_phys_addr);
    uint32_t total_size = *(uint32_t*)mbi_virt_addr;
    uintptr_t mbi_end = mbi_virt_addr + total_size;

    struct mb2_tag *tag;
    
    for (tag = (struct mb2_tag *)(mbi_virt_addr + 8);
         (uintptr_t)tag < mbi_end && tag->type != 0;
         tag = (struct mb2_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) 
    {
        if (tag->type == 6) { 
            struct mb2_tag_mmap *mmap = (struct mb2_tag_mmap *)tag;
            struct mb2_mmap_entry *entry;

            for (entry = mmap->entries;
                 (uint8_t *)entry < (uint8_t *)tag + tag->size;
                 entry = (struct mb2_mmap_entry *)((uint8_t *)entry + mmap->entry_size)) 
            {
                if (entry->type == 1) {
                    for (uintptr_t a = entry->addr; a < entry->addr + entry->len; a += PAGE_SIZE) {
                        if (a < MAX_BLOCKS * PAGE_SIZE) {
                            pmm_mark_free(a);
                        }
                    }
                }
            }
        }
    }

    uintptr_t k_start_phys = V2P((uintptr_t)_kernel_start);
    uintptr_t k_end_phys = V2P((uintptr_t)_kernel_end);
    
    for (uintptr_t a = k_start_phys; a < k_end_phys; a += PAGE_SIZE) {
        pmm_mark_used(a);
    }
    
    for (uintptr_t a = 0; a < 0x100000; a += PAGE_SIZE) {
        pmm_mark_used(a);
    }

    pmm_mark_used(mbi_phys_addr);
}

void* pmm_alloc_block() {
    for (uint32_t i = 0; i < total_blocks; i++) {
        if (!(pmm_bitmap[i / 8] & (1 << (i % 8)))) {
            
            uintptr_t phys_addr = i * PAGE_SIZE;
            pmm_mark_used(phys_addr);
            
            return (void*)phys_addr;
        }
    }
    return NULL; 
}

void pmm_free_block(void* ptr) {
    pmm_mark_free((uintptr_t)ptr);
}