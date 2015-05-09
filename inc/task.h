#ifndef __TASK_H
#define __TASK_H

#include <type.h>
#include <pmm.h>
#include <vmm.h>

typedef int32_t pid_t;

enum task_state {
    TASK_UNINIT = 0,
    TASK_SLEEPING = 1,
    TASK_RUNABLE = 2,
    TASK_ZOMBIE = 3
};

/* kernel thread context */
struct context {
    uint32_t esp;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
};

struct mm_struct {
    pgd_t *pgd_dir;
};

struct proc_ctrl_blk {
    __volatile__ enum task_state state;
    pid_t pid;
    void *stack;
    struct mm_struct *mm;
    struct context context;     // context 
    struct proc_ctrl_blk *next;   // next task
};

extern pid_t now_pid;

// TODO int (*fn)(void *
int32_t kernel_thread(int (*fn)(void *), void *arg);

void kthread_exit();

#endif
