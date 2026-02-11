#include <LySys/LySystem.h>
#include <LySys/types.h>
#include <LySys/string.h>
#include <LySys/mbi.h>
#include <LySys/file/psf.h>
#include <stdarg.h>

static uint32_t term_x = 0;
static uint32_t term_y = 0;
static uint32_t term_color = 0xFFFFFF;

void scroll() {
    struct psf1_header *font = (struct psf1_header *)&_binary_zap_light16_psf_start;
    uintptr_t fb_virtual = (uintptr_t)fb.addr + KERNEL_VIRTUAL_BASE;
    uint32_t line_size = font->charsize * fb.pitch;
    uint32_t total_scroll_size = (fb.height - font->charsize) * fb.pitch;

    memcpy((void*)fb_virtual, (void*)(fb_virtual + line_size), total_scroll_size);

    uint32_t *last_line = (uint32_t*)(fb_virtual + total_scroll_size);
    for (uint32_t i = 0; i < line_size / 4; i++) {
        last_line[i] = 0x000000;
    }

    term_y -= font->charsize;
}

int putchar(int c) {
    struct psf1_header *font = (struct psf1_header *)&_binary_zap_light16_psf_start;

    if (c == '\n') {
        term_x = 0;
        term_y += font->charsize;
    } 
    else if (c == '\r') {
        term_x = 0;
    }
    else if (c == '\t') {
        term_x += (8 * 4);
    }
    else if (c == '\b') {
        if (term_x >= 8) {
            term_x -= 8; 
            draw_rect(term_x, term_y, 8, font->charsize, 0x000000);
        }
    }
    else {
        draw_char(c, term_x, term_y, term_color);
        term_x += 8;
    }

    if (term_x + 8 > fb.width) {
        term_x = 0;
        term_y += font->charsize;
    }

    if (term_y + font->charsize > fb.height) {
        scroll();
    }

    return c;
}

int vsprintk(char *str, const char *fmt, va_list args) {
    char *ptr = str;
    char buf[64];

    while (*fmt) {
        if (*fmt != '%') {
            *ptr++ = *fmt++;
            continue;
        }

        fmt++;
        switch (*fmt) {
            case 's': {
                char *s = va_arg(args, char *);
                if (!s) s = "(null)";
                while (*s) *ptr++ = *s++;
                break;
            }
            case 'd':
            case 'i': {
                itoa(va_arg(args, int64_t), buf, 10);
                char *t = buf;
                while (*t) *ptr++ = *t++;
                break;
            }
            case 'x':
            case 'X': {
                itoa(va_arg(args, uint64_t), buf, 16);
                char *t = buf;
                while (*t) *ptr++ = *t++;
                break;
            }
            case 'c':
                *ptr++ = (char)va_arg(args, int);
                break;
            case '%':
                *ptr++ = '%';
                break;
            default:
                *ptr++ = *fmt;
                break;
        }
        fmt++;
    }
    *ptr = '\0';
    return (int)(ptr - str);
}

int printk(const char *fmt, ...) {
    char print_buffer[2048];
    va_list args;
    va_start(args, fmt);

    int len = vsprintk(print_buffer, fmt, args);
    
    for (int i = 0; i < len; i++) {
        putchar(print_buffer[i]);
    }

    va_end(args);
    return len;
}