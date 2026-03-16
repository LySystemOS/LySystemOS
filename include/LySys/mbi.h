#ifndef MBI_H
#define MBI_H

#include <LySys/types.h>

struct mb2_tag {
    uint32_t type;
    uint32_t size;
};

struct mb2_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
} __attribute__((packed));

struct mb2_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct mb2_mmap_entry entries[0];
} __attribute__((packed));

struct mb2_fb_common {
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint16_t reserved;
} __attribute__((packed));

struct mb2_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    struct mb2_fb_common common;
} __attribute__((packed));

struct mb2_tag_old_acpi {
    uint32_t type;
    uint32_t size;
    uint8_t rsdp[0];
} __attribute__((packed));

struct mb2_tag_new_acpi {
    uint32_t type;
    uint32_t size;
    uint8_t rsdp[0];
} __attribute__((packed));

struct framebuffer_info {
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
};

extern struct framebuffer_info fb;

void mbi_parse(uint32_t addr);

#endif