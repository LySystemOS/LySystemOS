#include <LySys/paging.h>
#include <LySys/pmm.h>
#include <LySys/mbi.h>
#include <LySys/LySystem.h>

extern char _kernel_start[];
extern char _kernel_end[];
extern struct framebuffer_info fb;

#define KERNEL_OFFSET 0xFFFFFFFF80000000
#define P2V(addr) ((uint64_t)(addr) + KERNEL_OFFSET)

uint64_t* kernel_pml4;

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

uint64_t safe_pmm_alloc() {
    uint64_t addr = (uintptr_t)pmm_alloc_block();
    uint64_t phys_kernel_end = (uint64_t)((uintptr_t)_kernel_end - KERNEL_OFFSET);
    while (addr <= phys_kernel_end) {
        addr = (uintptr_t)pmm_alloc_block();
    }
    return addr;
}

void map_page(uint64_t virtual, uint64_t physical, uint64_t flags) {
    uint64_t indices[3] = {
        (virtual >> 39) & 0x1FF,
        (virtual >> 30) & 0x1FF,
        (virtual >> 21) & 0x1FF
    };
    uint64_t pt_idx = (virtual >> 12) & 0x1FF;
    uint64_t* current = kernel_pml4;

    for(int i = 0; i < 3; i++) {
        if (!(current[indices[i]] & PAGE_PRESENT)) {
            uint64_t new_phys = safe_pmm_alloc(); 
            uint64_t* new_virt = (uint64_t*)P2V(new_phys);
            
            for(int j=0; j<512; j++) new_virt[j] = 0;
            
            current[indices[i]] = new_phys | PAGE_PRESENT | PAGE_WRITE;
            current = new_virt;
        } else {
            current = (uint64_t*)P2V(current[indices[i]] & ~0xFFF);
        }
    }
    
    current[pt_idx] = (physical & ~0xFFF) | flags | PAGE_PRESENT;
    __asm__ volatile("invlpg (%0)" :: "r" (virtual) : "memory");
}

void paging_init() {
    uint64_t phys_pml4 = safe_pmm_alloc();
    kernel_pml4 = (uint64_t*)P2V(phys_pml4);

    for (int i = 0; i < 512; i++) kernel_pml4[i] = 0;

    for (uint64_t p = 0; p < 512 * 1024 * 1024; p += 4096) {
        map_page(KERNEL_OFFSET + p, p, PAGE_WRITE);
    }

    for (uint64_t i = 0; i < 0x1000000; i += 4096) {
        map_page(i, i, PAGE_WRITE);
    }

    if (fb.addr != 0) {
        uint64_t fb_size = fb.pitch * fb.height;
        if (fb_size % 4096 != 0) fb_size += 4096 - (fb_size % 4096);

        for (uint64_t i = 0; i < fb_size; i += 4096) {
            uint64_t phys = fb.addr + i;
        
            map_page(phys, phys, PAGE_WRITE);

            map_page(KERNEL_OFFSET + phys, phys, PAGE_WRITE);
        }
    }

    struct gdt_ptr gp;
    __asm__ volatile("sgdt %0" : "=m"(gp));
    __asm__ volatile("mov %0, %%cr3" : : "r"(phys_pml4) : "memory");
    __asm__ volatile("lgdt %0" : : "m"(gp));
}