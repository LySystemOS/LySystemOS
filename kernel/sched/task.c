#include <LySys/LySystem.h>
#include <LySys/sched.h>
#include <LySys/errno.h>
#include <LySys/string.h>

struct Task TaskTable[MAX_TASK_COUNT];
int NextPID = 1;

int CreateTask(char name[MAX_TASK_NAME], pid_t ppid) {
    for (int i = 0; i < MAX_TASK_COUNT; i++) {
        if (TaskTable[i].state == TASK_EMPTY) {
            struct Task* t = &TaskTable[i];

            t->pid = NextPID++;
            t->ppid = ppid;
            t->state = TASK_WAITING;
            strncpy(t->name, name, MAX_TASK_NAME);

            return t->pid;
        }
    }
    return -ENOMEM;
}