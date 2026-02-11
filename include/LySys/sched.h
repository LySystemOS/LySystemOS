#pragma once

#include <LySys/types.h>

#define MAX_TASK_COUNT 128
#define MAX_TASK_NAME  32

typedef enum TaskState {
    TASK_EMPTY,
    TASK_WAITING,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_ZOMBIE
} TaskState;

typedef struct Task {
    pid_t pid;
    pid_t ppid;
    char name[MAX_TASK_NAME];
    TaskState state;
} Task;

extern struct Task task_table[MAX_TASK_COUNT];