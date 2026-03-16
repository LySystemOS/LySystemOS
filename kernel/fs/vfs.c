#include <LySys/vfs.h>
#include <LySys/fs/minix.h>

filesystem_t *active_fs = &minix_driver;

void ListDir(char* name) {
    active_fs->ListDir(name);
}

int CreateFile(char* name) {
    return active_fs->CreateFile(name);
}

int DeleteFile(char* name) {
    return active_fs->DeleteFile(name);
}
int ReadFile(char* name, char *output, int count) {
    return active_fs->ReadFile(name, output, count);
}
int WriteFile(char* name, const char* input, int count) {
    return active_fs->WriteFile(name, input, count);
}

int RenameFile(char* old_name, char* new_name) {
    return active_fs->RenameFile(old_name, new_name);
}

int CopyFile(char* source, char* dest) {
    return active_fs->CopyFile(source, dest);
}

int fs_init() {
    return minixfs_init();
}