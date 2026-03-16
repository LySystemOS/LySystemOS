#pragma once

#include <LySys/types.h>

void reverse(char* str, size_t len);
char* itoa(uint64_t value, char* str, int base);
int strcmp(const char* s1, const char* s2);
void* memcpy(void* dest, const void* src, size_t n);
void *memset(void *s, int c, size_t n);
size_t strlen(const char *str);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strrchr(const char* s, int c);
char* strchr(const char* s, int c);
char* strtok(char* str, const char* delimiters);