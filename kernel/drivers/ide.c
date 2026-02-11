#include <LySys/file/ide.h>
#include <LySys/LySystem.h>
#include <asm/io.h>

extern volatile uint8_t ide_irq_fired;

void ide_wait_bsy() {
    while (inb(ATA_PRIMARY_COMM_STAT) & 0x80);
}

void ide_wait_irq() {
    while (!ide_irq_fired) {
        __asm__ volatile("hlt");
    }
    ide_irq_fired = 0;
}

void ide_read_sector(uint32_t lba, uint8_t *buffer) {
    outb(0x3F6, 0x00);
    ide_wait_bsy();

    outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, 1);
    outb(ATA_PRIMARY_LBA_LOW, (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)(lba >> 16));
    
    ide_irq_fired = 0;
    outb(ATA_PRIMARY_COMM_STAT, ATA_CMD_READ_PIO);

    ide_wait_irq();

    uint16_t *ptr = (uint16_t *)buffer;
    for (int i = 0; i < 256; i++) {
        ptr[i] = inw(ATA_PRIMARY_DATA);
    }
}

void ide_write_sector(uint32_t lba, uint8_t *buffer) {
    outb(0x3F6, 0x00);
    ide_wait_bsy();

    outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, 1);
    outb(ATA_PRIMARY_LBA_LOW, (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)(lba >> 16));
    
    ide_irq_fired = 0;
    outb(ATA_PRIMARY_COMM_STAT, 0x30); 

    ide_wait_bsy();

    uint16_t *ptr = (uint16_t *)buffer;
    for (int i = 0; i < 256; i++) {
        outw(ATA_PRIMARY_DATA, ptr[i]);
    }

    outb(ATA_PRIMARY_COMM_STAT, 0xE7); 
    ide_wait_bsy();
}