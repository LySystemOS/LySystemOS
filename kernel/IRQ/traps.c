#include <LySys/LySystem.h>
#include <LySys/sched.h>
#include <LySys/types.h>
#include <LySys/time.h>
#include <LySys/sched.h>
#include <LySys/sys.h>
#include <LySys/video.h>
#include <asm/io.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

struct idt_entry_t {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

extern struct idt_entry_t idt_table[256];
extern void irq0_handler();
extern char keyboard_transform(uint8_t code);
extern uint64_t ticks;
extern struct tm time;
extern int mouse_x, mouse_y, mouse_old_x, mouse_old_y;

uint8_t mouse_cycle = 0;
int8_t mouse_packet[3];

struct interrupt_frame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t rip, cs, rflags, rsp, ss;
};

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x00); 
    outb(0xA1, 0x00); 
}

void set_idt_gate(int vector, void* handler, uint8_t dpl) {
    uint64_t addr = (uint64_t)handler;
    idt_table[vector].offset_low  = (uint16_t)(addr & 0xFFFF);
    idt_table[vector].selector    = 0x08;
    idt_table[vector].ist         = 0;
    idt_table[vector].attributes  = (uint8_t)(0x8E | ((dpl & 0x3) << 5)); 
    idt_table[vector].offset_mid  = (uint16_t)((addr >> 16) & 0xFFFF);
    idt_table[vector].offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    idt_table[vector].reserved    = 0;
}

__attribute__((interrupt)) 
void divide_by_zero_handler(struct interrupt_frame* frame) {
    if (CurrentTaskPID == 0) {
        panic("Divide By Zero in idle thread.");
    } else {
        ExitTask(-1);
    }
}

__attribute__((interrupt)) 
void general_protection_handler(struct interrupt_frame* frame, uint64_t error_code) {
    if (CurrentTaskPID == 0) {
        panic("General Protection Fault in idle thread.");
    } else {
        KillTask(CurrentTaskPID);
        Schedule();
    }
}

__attribute__((interrupt)) 
void page_handler(struct interrupt_frame* frame, uint64_t error_code) {
    if (CurrentTaskPID == 0) {
        panic("Page Fault in idle thread.");
    } else {
        KillTask(CurrentTaskPID);
        Schedule();
    }
}

__attribute__((interrupt))
void keyboard_handler(struct interrupt_frame* frame) {
    __asm__("cli");
    uint8_t scancode = inb(0x60);
    char c = keyboard_transform(scancode);
    if (c != 0) { printk("%c", c); }
    outb(0x20, 0x20);
    __asm__("sti");
}

uint32_t mouse_under_pixel = 0; 
int mouse_initialized = 0;
#define MOUSE_W 11
#define MOUSE_H 18
uint8_t mouse_cursor_shape[MOUSE_W * MOUSE_H] = {
    1,0,0,0,0,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,0,0,0,
    1,2,1,0,0,0,0,0,0,0,0,
    1,2,2,1,0,0,0,0,0,0,0,
    1,2,2,2,1,0,0,0,0,0,0,
    1,2,2,2,2,1,0,0,0,0,0,
    1,2,2,2,2,2,1,0,0,0,0,
    1,2,2,2,2,2,2,1,0,0,0,
    1,2,2,2,2,2,2,2,1,0,0,
    1,2,2,2,2,2,2,2,2,1,0,
    1,2,2,2,2,2,1,1,1,1,1,
    1,2,2,1,2,2,1,0,0,0,0,
    1,2,1,0,1,2,2,1,0,0,0,
    1,1,0,0,1,2,2,1,0,0,0,
    0,0,0,0,0,1,2,2,1,0,0,
    0,0,0,0,0,1,2,2,1,0,0,
    0,0,0,0,0,0,1,1,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0
};
uint32_t mouse_bg_buffer[MOUSE_W * MOUSE_H];

__attribute__((interrupt))
void mouse_handler(struct interrupt_frame* frame) {
    uint8_t status = inb(0x64);
    if (!(status & 0x20)) goto end;
    uint8_t data = inb(0x60);
    mouse_packet[mouse_cycle++] = data;
    if (mouse_cycle < 3) goto end;
    if (!(mouse_packet[0] & 0x08)) { mouse_cycle = 0; goto end; }
    int dx = mouse_packet[1];
    int dy = -mouse_packet[2];

    if (mouse_initialized) {
        for (int y = 0; y < MOUSE_H; y++) {
            for (int x = 0; x < MOUSE_W; x++) {
                if ((mouse_x + x) < WIDTH_SCREEN && (mouse_y + y) < HEIGHT_SCREEN) {
                    put_pixel(mouse_x + x, mouse_y + y, mouse_bg_buffer[y * MOUSE_W + x]);
                }
            }
        }
    }
    mouse_x += dx;
    mouse_y += dy;
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x >= WIDTH_SCREEN - 1)  mouse_x = WIDTH_SCREEN - 1;
    if (mouse_y >= HEIGHT_SCREEN - 1) mouse_y = HEIGHT_SCREEN - 1;
    mouse_cycle = 0;
    for (int y = 0; y < MOUSE_H; y++) {
        for (int x = 0; x < MOUSE_W; x++) {
            if ((mouse_x + x) < WIDTH_SCREEN && (mouse_y + y) < HEIGHT_SCREEN) {
                mouse_bg_buffer[y * MOUSE_W + x] = get_pixel(mouse_x + x, mouse_y + y);
            }
        }
    }
    for (int y = 0; y < MOUSE_H; y++) {
        for (int x = 0; x < MOUSE_W; x++) {
            if ((mouse_x + x) >= WIDTH_SCREEN || (mouse_y + y) >= HEIGHT_SCREEN) continue;
            uint8_t type = mouse_cursor_shape[y * MOUSE_W + x];
            if (type == 1) { put_pixel(mouse_x + x, mouse_y + y, 0x000000); } 
            else if (type == 2) { put_pixel(mouse_x + x, mouse_y + y, 0xFFFFFF); }
        }
    }
    mouse_initialized = 1;
end:
    outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

void timer_callback(struct interrupt_frame* frame) {
    outb(0x20, 0x20);
    ticks++;
    Schedule(); 
}

void syscall_interrupt_handler(struct interrupt_frame* frame) {
    uint64_t rax_val, rdx_val, rdi_val, rsi_val;
    __asm__ volatile("mov %%rax, %0" : "=r"(rax_val));
    __asm__ volatile("mov %%rdx, %0" : "=r"(rdx_val));
    __asm__ volatile("mov %%rdi, %0" : "=r"(rdi_val));
    __asm__ volatile("mov %%rsi, %0" : "=r"(rsi_val));
    for(;;);
}

volatile uint8_t ide_irq_fired = 0;
__attribute__((interrupt))
void ide_handler(struct interrupt_frame* frame) {
    ide_irq_fired = 1;
    inb(0x1F7);
    inb(0x177);
    outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

__attribute__((interrupt))
void sci_handler(struct interrupt_frame* frame) {
    outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

void irq_init() {
    pic_remap();
    set_idt_gate(32, (void*)irq0_handler, 0);
    set_idt_gate(33, (void*)keyboard_handler, 0);
    set_idt_gate(41, (void*)sci_handler, 0);
    set_idt_gate(44, (void*)mouse_handler, 0);
    set_idt_gate(46, (void*)ide_handler, 0);
    set_idt_gate(47, (void*)ide_handler, 0);
    outb(0x64, 0xA8);
    outb(0x64, 0x20);
    while (!(inb(0x64) & 1)); 
    uint8_t b = inb(0x60) | 2;
    b &= ~0x20;
    outb(0x64, 0x60);
    while (inb(0x64) & 2);
    outb(0x60, b);
    outb(0x64, 0xD4);
    while (inb(0x64) & 2);
    outb(0x60, 0xF4);
    while (!(inb(0x64) & 1));
    inb(0x60);
    outb(0xA1, inb(0xA1) & ~(1 << 1));
}

void traps_init() {
    __asm__ volatile("cli");
    set_idt_gate(0,  (void*)divide_by_zero_handler, 0);
    set_idt_gate(13, (void*)general_protection_handler, 0);
    set_idt_gate(14, (void*)page_handler, 0);
    set_idt_gate(0x80, (void*)syscall_interrupt_handler, 3);
    irq_init();
    __asm__ volatile("sti");
}