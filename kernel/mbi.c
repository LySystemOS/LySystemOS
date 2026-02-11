#include <LySys/mbi.h>
#include <LySys/LySystem.h>
#include <LySys/pmm.h>

struct framebuffer_info fb;

#define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000 

#define P2V(addr) ((uintptr_t)(addr) + KERNEL_VIRTUAL_BASE)

void mbi_parse(uint32_t phys_addr) {
    struct mb2_tag *tag;

    uintptr_t mbi_base_virt = P2V(phys_addr);
    uint32_t total_size = *(uint32_t *)mbi_base_virt;
    uintptr_t mbi_end_virt = mbi_base_virt + total_size;

    for (tag = (struct mb2_tag *)(mbi_base_virt + 8);
         (uintptr_t)tag < mbi_end_virt && tag->type != 0; 
         tag = (struct mb2_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) 
    {
        switch (tag->type) {
            case 6: { 
                struct mb2_tag_mmap *mmap = (struct mb2_tag_mmap *)tag;
                struct mb2_mmap_entry *entry;
                
                for (entry = mmap->entries;
                     (uint8_t *)entry < (uint8_t *)tag + tag->size;
                     entry = (struct mb2_mmap_entry *)((uint8_t *)entry + mmap->entry_size)) 
                {
                    if (entry->type == 1) {
                         for (uintptr_t a = entry->addr; a < entry->addr + entry->len; a += 4096) {
                             pmm_mark_free(a);
                         }
                    }
                }
                break;
            }
            case 8: { 
                struct mb2_tag_framebuffer *fb_tag = (struct mb2_tag_framebuffer *)tag;
                
                fb.addr = fb_tag->common.framebuffer_addr;
                fb.pitch = fb_tag->common.framebuffer_pitch;
                fb.width = fb_tag->common.framebuffer_width;
                fb.height = fb_tag->common.framebuffer_height;
                fb.bpp = fb_tag->common.framebuffer_bpp;
                break;
            }
        }
    }
}