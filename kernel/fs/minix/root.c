#include <LySys/fs/minix.h>
#include <LySys/file/ide.h>
#include <LySys/LySystem.h>

void minix_GetRootInode() {
    uint8_t buffer[512];
    
    ide_read_sector(10, buffer);
    
    struct minix2_inode *root = (struct minix2_inode *)(buffer);
    
    printk("Root Inode Mode: %x\n", root->i_mode);
    printk("Root Size: %d bytes\n", root->i_size);
    printk("Root First Data Zone: %d\n", root->i_zone[0]);
}

void minix_ListRoot() {
    uint8_t sector_buffer[1024]; 

    ide_read_sector(438, sector_buffer);
    ide_read_sector(439, sector_buffer + 512);

    struct minix_dir_entry *entry = (struct minix_dir_entry *)sector_buffer;
    
    for (int i = 0; i < 32; i++) {
        if (entry[i].inode != 0) {
            
            uint16_t target_inode = entry[i].inode;
            uint8_t temp_buf[512];
            
            uint32_t i_sector = 10 + ((target_inode - 1) / 16);
            uint32_t i_offset = ((target_inode - 1) % 16) * 32;
            
            ide_read_sector(i_sector, temp_buf);
            struct minix2_inode *inode_ptr = (struct minix2_inode *)(temp_buf + i_offset);
            
            int is_dir = (inode_ptr->i_mode & 0x4000);

            printk("%s", entry[i].name);
            
            if (is_dir) {
                printk("/");
            }
            printk("\n");
        }
    }
}