#ifndef SCHED_H
#define SCHED_H

#include <LySys/types.h>
#include <sys/user.h>

#define MAX_TASK_COUNT 128
#define MAX_TASK_NAME  32

typedef enum TaskState {
    TASK_EMPTY,
    TASK_RUNNING,
    TASK_WAITING,
    TASK_BLOCKED,
    TASK_SLEEPING,
    TASK_ZOMBIE
} TaskState;

typedef struct Task {
    pid_t pid;
    pid_t ppid;
    char name[MAX_TASK_NAME];
    TaskState state;
    int priority;
    int ExitCode;
    uint64_t rsp;
    uint64_t kstack;
    cputime_t time;
    uint64_t entry_point;
    uint64_t msleep;
    void* private_data;
} Task;

extern struct Task TaskTable[MAX_TASK_COUNT];
extern int TaskCount;
extern int CurrentTaskPID;

void sched_init();
int CreateTask(const char *name, void (*task)(void), pid_t ppid);
void Schedule();
void ListTask();
int ExitTask(int ExitCode);
int KillTask(pid_t pid);
int TaskFork();
extern void switch_to(uint64_t *prev_rsp, uint64_t next_rsp);

#endif