#include <LySys/fs/minix.h>
#include <LySys/file/ide.h>
#include <LySys/string.h>

#define INODE_BITMAP_LBA 2

struct minix2_inode minix_GetInode(uint16_t inode_no) {
    uint8_t buf[512];
    uint32_t sector = 10 + ((inode_no - 1) / 16);
    uint32_t offset = ((inode_no - 1) % 16) * 32;
    struct minix2_inode inode;

    ide_read_sector(sector, buf);
    memcpy(&inode, buf + offset, sizeof(struct minix2_inode));

    return inode;
}

void minix_ReadInode(uint16_t inode_no, struct minix2_inode *inode) {
    uint8_t buf[512];
    uint32_t sector = 10 + ((inode_no - 1) / 16);
    uint32_t offset = ((inode_no - 1) % 16) * 32;
    ide_read_sector(sector, buf);
    memcpy(inode, buf + offset, sizeof(struct minix2_inode));
}

void minix_WriteInode(uint16_t inode_no, struct minix2_inode *inode) {
    uint8_t buf[512];
    uint32_t sector = 10 + ((inode_no - 1) / 16);
    uint32_t offset = ((inode_no - 1) % 16) * 32;
    ide_read_sector(sector, buf);
    memcpy(buf + offset, inode, sizeof(struct minix2_inode));
    ide_write_sector(sector, buf);
}

uint16_t minix_AllocateInode() {
    uint8_t bitmap[512];
    ide_read_sector(INODE_BITMAP_LBA, bitmap);
    for (int i = 0; i < 512; i++) {
        if (bitmap[i] != 0xFF) {
            for (int bit = 0; bit < 8; bit++) {
                if (!(bitmap[i] & (1 << bit))) {
                    uint16_t found = (i * 8) + bit + 1;
                    if (found <= 2) continue;
                    bitmap[i] |= (1 << bit);
                    ide_write_sector(INODE_BITMAP_LBA, bitmap);
                    return found;
                }
            }
        }
    }
    return 0;
}

void minix_FreeInode(uint16_t inode_no) {
    if (inode_no == 0) return;
    uint8_t buf[512];
    uint16_t effective_no = inode_no - 1;
    ide_read_sector(INODE_BITMAP_LBA, buf);
    buf[effective_no / 8] &= ~(1 << (effective_no % 8));
    ide_write_sector(INODE_BITMAP_LBA, buf);
}