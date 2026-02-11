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
    uint16_t i_mode;        // Offset 0: Dosya Tipi ve İzinler
    uint16_t i_uid;         // Offset 2: Kullanıcı ID
    uint32_t i_size;        // Offset 4: DOSYA BOYUTU (Burada olmalı!)
    uint32_t i_mtime;       // Offset 8: Zaman Damgası
    uint8_t  i_gid;         // Offset 12: Grup ID
    uint8_t  i_nlinks;      // Offset 13: Link Sayısı
    uint16_t i_zone[7];     // Offset 14: Veri Blokları (Zone 0-6)
    uint16_t i_indir_zone;  // Offset 28: Dolaylı Blok
    uint16_t i_double_indir;// Offset 30: Çift Dolaylı Blok
} __attribute__((packed));  // TOPLAM: 32 Byte

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