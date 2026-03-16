#include <LySys/vfs.h>
#include <LySys/fs/minix.h>
#include <LySys/errno.h>

#define INODE_BITMAP_LBA 2
#define ZONE_BITMAP_LBA  3 

filesystem_t minix_driver = {
    .name = "minix2",
    .ReadFile = minix_ReadFile,
    .WriteFile = minix_WriteFile,
    .CreateFile = minix_CreateFile,
    .DeleteFile = minix_DeleteFile,
    .RenameFile = minix_RenameFile,
    .CopyFile = minix_CopyFile,
    .CreateDir = minix_CreateDir,
    .DeleteDir = minix_DeleteDir,
    .ListDir = minix_ListDir,
    .fs_init = minixfs_init
};
int minixfs_init() {
    int sup = minix_GetSuperBlock();
    if (sup == -EINVAL) {
        return -EINVAL;
    }
    else {
        minix_GetRootInode();
    }
    return ESUCCESS;
}
