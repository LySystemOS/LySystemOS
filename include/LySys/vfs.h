#pragma once

#include <LySys/types.h>

typedef struct filesystem {
    char name[16];
    int (*ReadFile)(char* name, char* output, int count);
    int (*WriteFile)(char* name, const char* input, int count);
    int (*CreateFile)(char* name);
    int (*DeleteFile)(char* name);
    int (*RenameFile)(char* old_name, char* new_name);
    int (*CopyFile)(char* source, char* dest);
    int (*CreateDir)(char* name);
    int (*DeleteDir)(char* name);
    void (*ListDir)(char* name);
    int (*fs_init)();
} filesystem_t;

extern filesystem_t minix_driver;

void ListDir(char* name);
int CreateFile(char* name);
int DeleteFile(char* name);
int ReadFile(char* name, char *output, int count);
int WriteFile(char* name, const char* input, int count);
int RenameFile(char* old_name, char* new_name);
int CopyFile(char* source, char* dest);
int fs_init();
