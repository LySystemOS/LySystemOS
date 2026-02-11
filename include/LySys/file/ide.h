#ifndef IDE_H
#define IDE_H

#include <LySys/types.h>

#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERR          0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LOW      0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HIGH     0x1F5
#define ATA_PRIMARY_DRIVE_SEL    0x1F6
#define ATA_PRIMARY_COMM_STAT    0x1F7

#define ATA_CMD_READ_PIO         0x20
#define ATA_CMD_WRITE_PIO        0x30
#define ATA_CMD_IDENTIFY         0xEC

void ide_write_sector(uint32_t lba, uint8_t *buffer);
void ide_read_sector(uint32_t lba, uint8_t *buffer);
void ide_wait_irq();
void ide_wait_bsy();

#endif