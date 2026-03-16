#pragma once

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;

typedef unsigned long long size_t;
typedef long long          ssize_t;
typedef unsigned long long uintptr_t;
typedef long long          intptr_t;
typedef uint64_t           phys_addr_t;
typedef uint64_t           virt_addr_t;
typedef int                pid_t;
typedef uint64_t           cputime_t;

typedef _Bool bool;
#define true  1
#define false 0
#define NULL  ((void*)0)

#define UINT8_MAX  0xff
#define UINT16_MAX 0xffff
#define UINT32_MAX 0xffffffff
#define UINT64_MAX 0xffffffffffffffffULL

#define PACKED     __attribute__((packed))
#define ALIGNED(x) __attribute__((aligned(x)))
#define NORETURN   __attribute__((noreturn))
#define INLINE     static inline

typedef void (*void_func_t)(void);