#include <LySys/heap.h>
#include <LySys/pmm.h>
#include <LySys/paging.h>
#include <LySys/LySystem.h>

static heap_header_t* heap_start = NULL;

void heap_init(uint64_t start_addr, uint64_t end_addr) {
    uint64_t page_aligned_start = start_addr & ~0xFFF;
    uint64_t page_aligned_end = (end_addr + 0xFFF) & ~0xFFF;

    for (uint64_t v = page_aligned_start; v < page_aligned_end; v += 4096) {
        uint64_t phys = (uintptr_t)pmm_alloc_block();
        
        if (!phys) return;

        map_page(v, phys, PAGE_PRESENT | PAGE_WRITE);
        __asm__ volatile("invlpg (%0)" :: "r" (v) : "memory");
    }

    heap_start = (heap_header_t*)start_addr;
    heap_start->magic = HEAP_MAGIC;
    heap_start->is_free = 1;
    heap_start->size = (end_addr - start_addr) - sizeof(heap_header_t);
    heap_start->next = NULL;
}

void* kmalloc(uint64_t size) {
    size = (size + 15) & ~15;
    
    heap_header_t* current = heap_start;

    while (current) {
        if (current->is_free && current->size >= size) {
            if (current->size > size + sizeof(heap_header_t) + 32) {
                heap_header_t* next_block = (heap_header_t*)((uint64_t)current + sizeof(heap_header_t) + size);
                next_block->magic = HEAP_MAGIC;
                next_block->is_free = 1;
                next_block->size = current->size - size - sizeof(heap_header_t);
                next_block->next = current->next;

                current->size = size;
                current->next = next_block;
            }
            
            current->is_free = 0;
            return (void*)((uint64_t)current + sizeof(heap_header_t));
        }
        current = current->next;
    }

    return NULL;
}

void kfree(void* ptr) {
    if (!ptr) return;

    heap_header_t* header = (heap_header_t*)((uint64_t)ptr - sizeof(heap_header_t));
    if (header->magic != HEAP_MAGIC) return;

    header->is_free = 1;

    heap_header_t* current = heap_start;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            current->size += current->next->size + sizeof(heap_header_t);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}