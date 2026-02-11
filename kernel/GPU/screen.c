#include <LySys/LySystem.h>
#include <LySys/video.h>
#include <LySys/mbi.h>
#include <LySys/file/psf.h>

void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb.width || y >= fb.height) return;
    
    if (fb.addr == 0) return;

    uintptr_t fb_virtual = (uintptr_t)fb.addr + KERNEL_VIRTUAL_BASE;
    
    uint8_t *pixel_addr = (uint8_t *)fb_virtual + (y * fb.pitch) + (x * (fb.bpp / 8));

    *(volatile uint32_t *)pixel_addr = color;
}

void clear_screen(uint32_t color) {
    if (fb.addr == 0) return;

    for (uint32_t y = 0; y < fb.height; y++) {
        for (uint32_t x = 0; x < fb.width; x++) {
            put_pixel(x, y, color);
        }
    }
    __asm__ volatile ("mfence" ::: "memory"); 
}

void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

void draw_char(char c, uint32_t x, uint32_t y, uint32_t color) {
    struct psf1_header *font = (struct psf1_header *)&_binary_zap_light16_psf_start;

    if (font->magic != PSF1_MAGIC) {
        return;
    }

    uint8_t *glyph = (uint8_t *)&_binary_zap_light16_psf_start + 4 + ((uint8_t)c * font->charsize);

    for (uint32_t cy = 0; cy < font->charsize; cy++) {
        for (uint32_t cx = 0; cx < 8; cx++) {
            if (glyph[cy] & (0x80 >> cx)) {
                put_pixel(x + cx, y + cy, color);
            }
        }
    }
}