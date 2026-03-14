#pragma once

#include <LySys/types.h>

#define INODE_BITMAP_LBA 2
#define ZONE_BITMAP_LBA  3 

struct minix2_superblock {
    uint16_t s_ninodes;
    uint16_t s_nzones;
    uint16_t s_imap_blocks;
    uint16_t s_zmap_blocks;
    uint16_t s_firstdatazone;
    uint16_t s_log_zone_size;
    uint32_t s_max_size;
    uint16_t s_magic;
    uint16_t s_state;
    uint32_t s_zones;
};

struct minix2_inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_mtime;
    uint8_t  i_gid;
    uint8_t  i_nlinks;
    uint16_t i_zone[7];
    uint16_t i_indir_zone;
    uint16_t i_double_indir;
} __attribute__((packed));

struct minix_dir_entry {
    uint16_t inode;
    char name[30];
} __attribute__((packed));

extern struct minix2_superblock current_sb;

int minix_GetSuperBlock();
void minix_GetRootInode();
void minix_ListRoot();
void minix_ReadInode(uint16_t inode_no, struct minix2_inode *inode);
void minix_WriteInode(uint16_t inode_no, struct minix2_inode *inode);
void minix_FreeInode(uint16_t inode_no);
struct minix2_inode minix_GetInode(uint16_t inode_no);
uint16_t minix_AllocateInode();
int minix_CreateFile(char* name);
int minix_DeleteFile(char* name);
int minix_ReadFile(char* name, char *output, int count);
int minix_WriteFile(char* name, const char* input, int count);
int minix_CreateDir(char* name);
int minix_DeleteDir(char* name);
int minix_RenameFile(char* old_name, char* new_name);
int minix_CopyFile(char* source, char* dest);
int minixfs_init();
