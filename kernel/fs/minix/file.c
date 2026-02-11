#include <LySys/fs/minix.h>
#include <LySys/file/ide.h>
#include <LySys/LySystem.h>
#include <LySys/string.h>
#include <LySys/errno.h>

#define INODE_BITMAP_LBA 2
#define ZONE_BITMAP_LBA  3 

void FreeZone(uint16_t zone_no) {
    if (zone_no == 0) return;

    uint8_t buf[512];
    ide_read_sector(ZONE_BITMAP_LBA, buf);

    uint32_t byte_idx = zone_no / 8;
    uint32_t bit_idx = zone_no % 8;

    buf[byte_idx] &= ~(1 << bit_idx);
    ide_write_sector(ZONE_BITMAP_LBA, buf);
}

uint16_t FindEntry(char* name) {
    uint8_t buf[512];
    ide_read_sector(438, buf);
    struct minix_dir_entry *entries = (struct minix_dir_entry *)buf;

    for (int i = 0; i < 16; i++) {
        if (entries[i].inode != 0) {
            if (strcmp(entries[i].name, name) == 0) {
                return entries[i].inode;
            }
        }
    }
    return 0;
}

int minix_CreateFile(char* name) {
    if (FindEntry(name) != 0) {
        return -EEXIST;
    }

    uint16_t new_inode_no = minix_AllocateInode();
    if (new_inode_no == 0) {
        return -ENOSPC;
    }

    uint8_t buf[512];
    uint32_t sector = 10 + ((new_inode_no - 1) / 16);
    uint32_t offset = ((new_inode_no - 1) % 16) * 32;

    ide_read_sector(sector, buf);
    struct minix2_inode *new_inode = (struct minix2_inode *)(buf + offset);
    
    memset(new_inode, 0, sizeof(struct minix2_inode));
    new_inode->i_mode = 0x81ED;
    new_inode->i_size = 0;
    new_inode->i_nlinks = 1;
    
    ide_write_sector(sector, buf);

    ide_read_sector(438, buf);
    struct minix_dir_entry *entries = (struct minix_dir_entry *)buf;
    for(int i = 0; i < 16; i++) {
        if(entries[i].inode == 0) {
            entries[i].inode = new_inode_no;
            strncpy(entries[i].name, name, 30);
            ide_write_sector(438, buf);
            return ESUCCESS;
        }
    }
}

int minix_DeleteFile(char* name) {
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        return -EACCES;
    }

    uint8_t buf[512];
    uint16_t target_inode = 0;
    int entry_index = -1;

    ide_read_sector(438, buf);
    struct minix_dir_entry *entries = (struct minix_dir_entry *)buf;

    for (int i = 0; i < 16; i++) {
        if (entries[i].inode != 0 && strcmp(entries[i].name, name) == 0) {
            target_inode = entries[i].inode;
            entry_index = i;
            break;
        }
    }

    if (target_inode == 0) {
        return -ENOENT;
    }

    uint8_t inode_buf[512];
    uint32_t sector = 10 + ((target_inode - 1) / 16);
    uint32_t offset = ((target_inode - 1) % 16) * 32;

    ide_read_sector(sector, inode_buf);
    struct minix2_inode *inode_ptr = (struct minix2_inode *)(inode_buf + offset);

    for (int z = 0; z < 7; z++) {
        if (inode_ptr->i_zone[z] != 0) {
            FreeZone(inode_ptr->i_zone[z]);
            inode_ptr->i_zone[z] = 0;
        }
    }
    memset(inode_ptr, 0, sizeof(struct minix2_inode));
    ide_write_sector(sector, inode_buf);

    ide_read_sector(INODE_BITMAP_LBA, buf);
    uint32_t byte_idx = target_inode / 8;
    uint32_t bit_idx = target_inode % 8;
    buf[byte_idx] &= ~(1 << bit_idx);
    ide_write_sector(INODE_BITMAP_LBA, buf);

    ide_read_sector(438, buf);
    struct minix_dir_entry *dir_entries = (struct minix_dir_entry *)buf;
    dir_entries[entry_index].inode = 0;
    memset(dir_entries[entry_index].name, 0, 30);
    ide_write_sector(438, buf);

    return ESUCCESS;
}

int minix_ReadFile(char* name, char *output, int count) {
    uint16_t inode_no = FindEntry(name);
    if (inode_no == 0) return -ENOENT;

    struct minix2_inode inode = minix_GetInode(inode_no);
    
    int actual_to_read = count;
    int padding_needed = 0;

    if (count > inode.i_size) {
        actual_to_read = inode.i_size;
        padding_needed = count - inode.i_size;
    }

    if (actual_to_read > 0 && inode.i_zone[0] != 0) {
        uint8_t sector_buf[512];
        uint32_t start_sector = inode.i_zone[0] * 2;
        ide_read_sector(start_sector, sector_buf);
        
        memcpy(output, sector_buf, actual_to_read);
    }

    if (padding_needed > 0) {
        memset(output + actual_to_read, 0, padding_needed);
    }

    return actual_to_read; 
}

int minix_WriteFile(char* name, const char* input, int count) {
    uint16_t inode_no = FindEntry(name);
    if (inode_no == 0) return -ENOENT;

    uint8_t inode_buf[512];
    uint32_t i_sector = 10 + ((inode_no - 1) / 16);
    uint32_t i_offset = ((inode_no - 1) % 16) * 32;
    ide_read_sector(i_sector, inode_buf);
    struct minix2_inode *inode = (struct minix2_inode *)(inode_buf + i_offset);

    if (inode->i_zone[0] == 0) {
        uint8_t z_bitmap[512];
        ide_read_sector(3, z_bitmap);
        
        uint16_t found_zone = 0;
        for (int i = 0; i < 512; i++) {
            if (z_bitmap[i] != 0xFF) {
                for (int bit = 0; bit < 8; bit++) {
                    if (!(z_bitmap[i] & (1 << bit))) {
                        z_bitmap[i] |= (1 << bit);
                        found_zone = (i * 8) + bit;
                        ide_write_sector(3, z_bitmap);
                        goto zone_found;
                    }
                }
            }
        }
        return -ENOSPC;

        zone_found:
        inode->i_zone[0] = found_zone;
    }

    uint8_t data_buf[512];
    uint32_t data_lba = inode->i_zone[0] * 2;
    
    memset(data_buf, 0, 512);
    int to_write = (count > 512) ? 512 : count;
    memcpy(data_buf, input, to_write);
    ide_write_sector(data_lba, data_buf);

    if (inode->i_size < to_write) {
        inode->i_size = to_write;
    }
    
    ide_write_sector(i_sector, inode_buf);

    return to_write;
}

int minix_CreateDir(char* name) {
    if (FindEntry(name) != 0) return -EEXIST;

    uint16_t new_inode_no = minix_AllocateInode();
    if (new_inode_no <= 2) return -ENOSPC;

    uint16_t new_zone = 0;
    uint8_t z_bitmap[512];
    ide_read_sector(3, z_bitmap);

    for (int i = 0; i < 512; i++) {
        if (z_bitmap[i] != 0xFF) {
            for (int bit = 0; bit < 8; bit++) {
                if (!(z_bitmap[i] & (1 << bit))) {
                    z_bitmap[i] |= (1 << bit);
                    new_zone = (i * 8) + bit;
                    ide_write_sector(3, z_bitmap);
                    goto zone_allocated;
                }
            }
        }
    }
    return -ENOSPC; 
    zone_allocated:
    uint8_t buf[512];
    uint32_t i_sector = 10 + ((new_inode_no - 1) / 16);
    uint32_t i_offset = ((new_inode_no - 1) % 16) * 32;

    ide_read_sector(i_sector, buf);
    struct minix2_inode *inode = (struct minix2_inode *)(buf + i_offset);
    
    memset(inode, 0, sizeof(struct minix2_inode));
    inode->i_mode = 0x41ED; 
    inode->i_size = 64;     
    inode->i_nlinks = 2;    
    inode->i_zone[0] = new_zone;
    
    ide_write_sector(i_sector, buf);

    uint8_t dir_data[512];
    memset(dir_data, 0, 512);
    struct minix_dir_entry *entries = (struct minix_dir_entry *)dir_data;

    entries[0].inode = new_inode_no;
    strncpy(entries[0].name, ".", 30);

    entries[1].inode = 1; 
    strncpy(entries[1].name, "..", 30);

    ide_write_sector(new_zone * 2, dir_data);

    ide_read_sector(438, buf);
    struct minix_dir_entry *root_entries = (struct minix_dir_entry *)buf;
    for(int i=0; i<16; i++) {
        if(root_entries[i].inode == 0) {
            root_entries[i].inode = new_inode_no;
            strncpy(root_entries[i].name, name, 30);
            ide_write_sector(438, buf);
            return ESUCCESS;
        }
    }
    return -ENOSPC;
}

void minix_ListDir(char* name) {
    uint16_t inode_no = 1;

    if (name != NULL && strcmp(name, "/") != 0) {
        inode_no = FindEntry(name);
    }
    
    if (inode_no == 0) {
        printk("Dizin bulunamadi.\n");
        return;
    }

    struct minix2_inode inode = minix_GetInode(inode_no);
    
    if ((inode.i_mode & 0x4000) != 0x4000) {
        printk("Hata: '%s' bir dizin degil!\n", name);
        return;
    }

    uint8_t buf[512];
    ide_read_sector(inode.i_zone[0] * 2, buf);
    struct minix_dir_entry *entries = (struct minix_dir_entry *)buf;

    printk("\n--- Dizin Icerigi: %s ---\n", (name) ? name : "/");
    for (int i = 0; i < 16; i++) {
        if (entries[i].inode != 0) {
            printk("[%d] %s\n", entries[i].inode, entries[i].name);
        }
    }
    printk("-------------------------\n");
}

int minix_DeleteDir(char* name) {
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return -EPERM;

    uint16_t inode_no = FindEntry(name);
    if (inode_no == 0) return -ENOENT;

    struct minix2_inode inode = minix_GetInode(inode_no);

    if ((inode.i_mode & 0x4000) != 0x4000) {
        return -ENOTDIR;
    }

    if (inode.i_size > 64) {
        return -EPERM; 
    }

    if (inode.i_zone[0] != 0) FreeZone(inode.i_zone[0]);
    
    uint8_t buf[512];
    uint32_t i_sector = 10 + ((inode_no - 1) / 16);
    uint32_t i_offset = ((inode_no - 1) % 16) * 32;
    ide_read_sector(i_sector, buf);
    memset(buf + i_offset, 0, sizeof(struct minix2_inode));
    ide_write_sector(i_sector, buf);

    ide_read_sector(2, buf);
    buf[inode_no / 8] &= ~(1 << (inode_no % 8));
    ide_write_sector(2, buf);

    ide_read_sector(438, buf);
    struct minix_dir_entry *root_entries = (struct minix_dir_entry *)buf;
    for(int i=0; i<16; i++) {
        if(root_entries[i].inode == inode_no) {
            root_entries[i].inode = 0;
            memset(root_entries[i].name, 0, 30);
            ide_write_sector(438, buf);
            break;
        }
    }

    return ESUCCESS;
}


int minix_RenameFile(char* old_name, char* new_name) {
    if (FindEntry(new_name) != 0) {
        return -EEXIST;
    }

    uint8_t buf[512];
    ide_read_sector(438, buf);
    struct minix_dir_entry *entries = (struct minix_dir_entry *)buf;

    for (int i = 0; i < 16; i++) {
        if (entries[i].inode != 0 && strcmp(entries[i].name, old_name) == 0) {
            
            memset(entries[i].name, 0, 30);
            strncpy(entries[i].name, new_name, 30);
            
            ide_write_sector(438, buf);
            return ESUCCESS;
        }
    }

    return -ENOENT;
}

int minix_CopyFile(char* source, char* dest) {
    uint8_t data_buffer[512];
    
    int bytes_read = minix_ReadFile(source, (char*)data_buffer, 512);
    if (bytes_read < 0) return bytes_read;

    int res = minix_CreateFile(dest);
    if (res != ESUCCESS) return res;

    int bytes_written = minix_WriteFile(dest, (char*)data_buffer, bytes_read);
    
    if (bytes_written != bytes_read) {
        return -1;
    }

    return ESUCCESS;
}