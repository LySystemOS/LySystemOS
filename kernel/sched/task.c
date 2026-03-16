#include <LySys/sched.h>
#include <LySys/string.h>
#include <LySys/LySystem.h>
#include <LySys/errno.h>
#include <LySys/video.h>
#include <LySys/heap.h>

struct Task TaskTable[MAX_TASK_COUNT];
int TaskCount = 0;
int CurrentTaskPID = -1;

static int scheduler_started = 0;

void sched_init() {
    memset(TaskTable, 0, sizeof(TaskTable));
    struct Task* t = &TaskTable[0];
    t->pid = 0;
    t->state = TASK_RUNNING;
    strncpy(t->name, "kernel_idle", MAX_TASK_NAME);
    t->priority = 0;
    t->kstack = 0; 
    
    CurrentTaskPID = 0;
    TaskCount = 1;
    
    scheduler_started = 1;
}

extern void thread_starter();
extern int ExitTask(int ExitCode);

int CreateTask(const char *name, void (*task)(void), pid_t ppid) {
    if (TaskCount >= MAX_TASK_COUNT) return -1;

    int id = -1;
    for(int i = 0; i < MAX_TASK_COUNT; i++) {
        if(TaskTable[i].state == TASK_EMPTY) {
            id = i;
            break;
        }
    }
    if (id == -1) return -1;

    struct Task* t = &TaskTable[id];
    t->pid = id;
    t->ppid = ppid;
    t->state = TASK_WAITING;
    t->priority = 1;
    strncpy(t->name, name, MAX_TASK_NAME - 1);

    t->kstack = (uint64_t)kmalloc(4096);
    if (!t->kstack) return -ENOMEM;

    uint64_t *stack_ptr = (uint64_t*)(t->kstack + 4096);

    *--stack_ptr = 0x10; 
    *--stack_ptr = (uint64_t)stack_ptr + 8; 
    *--stack_ptr = 0x202; 
    *--stack_ptr = 0x08; 
    *--stack_ptr = (uint64_t)task; 
    *--stack_ptr = (uint64_t)thread_starter; 
    *--stack_ptr = 0; 
    *--stack_ptr = 0; 
    *--stack_ptr = 0; 
    *--stack_ptr = 0; 
    *--stack_ptr = 0; 
    *--stack_ptr = 0; 

    t->rsp = (uint64_t)stack_ptr;
    TaskCount++;
    return id;
}

int TaskFork() {
    __asm__ volatile("cli");

    int id = -1;
    for (int i = 0; i < MAX_TASK_COUNT; i++) {
        if (TaskTable[i].state == TASK_EMPTY) {
            id = i;
            break;
        }
    }

    if (id == -1) {
        __asm__ volatile("sti");
        return -1;
    }

    struct Task* parent = &TaskTable[CurrentTaskPID];
    struct Task* child = &TaskTable[id];

    child->pid = id;
    child->ppid = parent->pid;
    child->state = TASK_WAITING;
    child->priority = parent->priority;
    strncpy(child->name, parent->name, MAX_TASK_NAME - 1);
    
    child->kstack = (uint64_t)kmalloc(4096);
    if (!child->kstack) {
        __asm__ volatile("sti");
        return -ENOMEM;
    }

    memcpy((void*)child->kstack, (void*)parent->kstack, 4096);

    uint64_t current_rsp;
    __asm__ volatile("mov %%rsp, %0" : "=r"(current_rsp));
    uint64_t offset = child->kstack - parent->kstack;
    uint64_t child_rsp_val = current_rsp + offset;

    uint64_t *sp = (uint64_t *)child_rsp_val;

    *--sp = (uint64_t)&&child_return; 
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;
    *--sp = 0;

    child->rsp = (uint64_t)sp;

    TaskCount++;
    __asm__ volatile("sti");

    return id;

child_return:
    __asm__ volatile("sti"); 
    return 0;
}

void Schedule() {
    if (scheduler_started == 0) return;

    for (int i = 0; i < MAX_TASK_COUNT; i++) {
        if (i > 0 && TaskTable[i].state == TASK_ZOMBIE) {
            if (TaskTable[i].kstack != 0) {
                kfree((void*)TaskTable[i].kstack);
            }
            memset(&TaskTable[i], 0, sizeof(struct Task));
            TaskTable[i].state = TASK_EMPTY;
            TaskCount--;
            continue;
        }

        if (TaskTable[i].state == TASK_SLEEPING) {
            if (TaskTable[i].msleep > 0) {
                TaskTable[i].msleep--;
            }
            if (TaskTable[i].msleep == 0) {
                TaskTable[i].state = TASK_WAITING;
            }
        }
    }

    int next_pid = -1;
    int max_priority = -1;

    for (int i = 0; i < MAX_TASK_COUNT; i++) {
        if (TaskTable[i].state == TASK_WAITING) {
            if (TaskTable[i].priority > max_priority) {
                max_priority = TaskTable[i].priority;
            }
        }
    }

    if (max_priority != -1) {
        for (int i = 1; i <= MAX_TASK_COUNT; i++) {
            int idx = (CurrentTaskPID + i) % MAX_TASK_COUNT;
            if (TaskTable[idx].state == TASK_WAITING && TaskTable[idx].priority == max_priority) {
                next_pid = idx;
                break;
            }
        }
    }

    if (next_pid == -1) {
        if (TaskTable[CurrentTaskPID].state == TASK_RUNNING) return;
        next_pid = 0;
    }

    if (next_pid == CurrentTaskPID) return;

    struct Task *prev = &TaskTable[CurrentTaskPID];
    struct Task *next = &TaskTable[next_pid];

    if (prev->state == TASK_RUNNING) {
        prev->state = TASK_WAITING;
    }
    
    next->state = TASK_RUNNING;
    CurrentTaskPID = next_pid;

    switch_to(&(prev->rsp), next->rsp);
}

void ListTask() {
    for (int i = 0; i < MAX_TASK_COUNT; i++) {
        if (TaskTable[i].state != TASK_EMPTY) {
            printk("[%d] Name:%s\n", TaskTable[i].pid, TaskTable[i].name);
        }
    }
}

int ExitTask(int ExitCode) {
    __asm__ volatile("cli");

    struct Task* t = &TaskTable[CurrentTaskPID];

    if (CurrentTaskPID == 0) {
        __asm__ volatile("sti");
        return -EACCES;
    }

    t->state = TASK_ZOMBIE;
    t->ExitCode = ExitCode;

    Schedule();

    while(1);
    return 0;
}

int KillTask(pid_t pid) {
    if (pid <= 0 || pid >= MAX_TASK_COUNT) return -EACCES;

    __asm__ volatile("cli");

    struct Task* target = &TaskTable[pid];

    if (target->state == TASK_EMPTY) {
        __asm__ volatile("sti");
        return -ESRCH;
    }

    if (pid == CurrentTaskPID) {
        __asm__ volatile("sti");
        ExitTask(-1);
    }

    target->state = TASK_ZOMBIE;
    target->ExitCode = -1;

    __asm__ volatile("sti");
    return 0;
}