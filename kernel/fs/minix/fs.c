#include <LySys/fs/minix.h>
#include <LySys/errno.h>

#define INODE_BITMAP_LBA 2
#define ZONE_BITMAP_LBA  3 

int minixfs_init() {
    int sup = minix_GetSuperBlock();
    if (sup == -EINVAL) {
        return -EINVAL;
    }
    else {
        minix_GetRootInode();
    }
}
