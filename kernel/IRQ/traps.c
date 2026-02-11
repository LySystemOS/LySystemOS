#include <LySys/LySystem.h>
#include <LySys/types.h>
#include <LySys/time.h>
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

extern char keyboard_transform(uint8_t code);
extern uint64_t ticks;
extern struct tm time;
extern void schedule();

struct interrupt_frame {
    uint64_t rip; 
    uint64_t cs; 
    uint64_t rflags; 
    uint64_t rsp; 
    uint64_t ss;
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
    printk("\nPANIC: DIV 0\n");
    while(1) { __asm__ volatile("hlt"); }
}

__attribute__((interrupt)) 
void general_protection_handler(struct interrupt_frame* frame, uint64_t error_code) {
    printk("\nPANIC: GPF %d\n", error_code);
    while(1) { __asm__ volatile("hlt"); }
}

__attribute__((interrupt)) 
void page_handler(struct interrupt_frame* frame, uint64_t error_code) {
    uint64_t cr2;
    __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
    printk("\nPANIC: PF 0x%x\n", cr2);
    while(1) { __asm__ volatile("hlt"); }
}

__attribute__((interrupt))
void keyboard_handler(struct interrupt_frame* frame) {
    uint8_t scancode = inb(0x60);

    
    char c = keyboard_transform(scancode);
    if (c != 0) {
            printk("%c", c);
    }

    outb(0x20, 0x20);
}

__attribute__((interrupt)) 
void timer_handler(struct interrupt_frame* frame) {
    ticks++;
    outb(0x20, 0x20);
}

__attribute__((interrupt))
void syscall_handler(struct interrupt_frame* frame) {
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

void irq_init() {
    pic_remap();
    set_idt_gate(32, (void*)timer_handler, 0);
    set_idt_gate(33, (void*)keyboard_handler, 0);
    set_idt_gate(46, (void*)ide_handler, 0);
    set_idt_gate(47, (void*)ide_handler, 0);
}

void traps_init() {
    __asm__ volatile("cli");
    
    set_idt_gate(0,  (void*)divide_by_zero_handler, 0);
    set_idt_gate(13, (void*)general_protection_handler, 0);
    set_idt_gate(14, (void*)page_handler, 0);
    set_idt_gate(0x80, (void*)syscall_handler, 0);
    
    irq_init();
    
    __asm__ volatile("sti");
}