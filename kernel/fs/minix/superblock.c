#include <LySys/fs/minix.h>
#include <LySys/file/ide.h>
#include <LySys/LySystem.h>
#include <LySys/errno.h>

struct minix2_superblock current_sb;

int minix_GetSuperBlock() {
    uint8_t buffer[512];
    
    ide_read_sector(2, buffer);
    
    struct minix2_superblock *sb_ptr = (struct minix2_superblock *)buffer;
    
    current_sb.s_ninodes = sb_ptr->s_ninodes;
    current_sb.s_nzones = sb_ptr->s_nzones;
    current_sb.s_imap_blocks = sb_ptr->s_imap_blocks;
    current_sb.s_zmap_blocks = sb_ptr->s_zmap_blocks;
    current_sb.s_firstdatazone = sb_ptr->s_firstdatazone;
    current_sb.s_log_zone_size = sb_ptr->s_log_zone_size;
    current_sb.s_max_size = sb_ptr->s_max_size;
    current_sb.s_magic = sb_ptr->s_magic;
    current_sb.s_state = sb_ptr->s_state;
    current_sb.s_zones = sb_ptr->s_zones;

    if (current_sb.s_magic == 0x2478 || current_sb.s_magic == 0x2479) {
        return ESUCCESS;
    } else {
        return -EINVAL;
    }
}