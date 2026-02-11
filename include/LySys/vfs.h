#pragma once

int GetSuperBlock();
void GetRootInode();
void ListRoot();
uint16_t AllocateInode();
int CreateFile(char* name);
int DeleteFile(char* name);
int ReadFile(char* name, char *output, int count);
int WriteFile(char* name, const char* input, int count);
int CreateDir(char* name);
int DeleteDir(char* name);
int RenameFile(char* old_name, char* new_name);
int CopyFile(char* source, char* dest);
int fs_init();