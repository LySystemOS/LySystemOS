#include <LySys/fs/minix.h>

int GetSuperBlock() {
    return minix_GetSuperBlock();
}
void GetRootInode() {
    minix_GetRootInode();
}
void ListRoot() {
    minix_ListRoot();
}
uint16_t AllocateInode() {
    return minix_AllocateInode();
}
int CreateFile(char* name) {
    return minix_CreateFile(name);
}
int DeleteFile(char* name) {
    return minix_DeleteFile(name);
}
int ReadFile(char* name, char *output, int count) {
    return minix_ReadFile(name, output, count);
}
int WriteFile(char* name, const char* input, int count) {
    return minix_WriteFile(name, input, count);
}
int CreateDir(char* name) {
    return minix_CreateDir(name);
}
int DeleteDir(char* name) {
    return minix_DeleteDir(name);
}
int RenameFile(char* old_name, char* new_name) {
    return minix_RenameFile(old_name, new_name);
}

int CopyFile(char* source, char* dest) {
    return minix_CopyFile(source, dest);
}

int fs_init() {
    return minixfs_init();
}