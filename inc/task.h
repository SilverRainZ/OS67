#ifndef __TASK_H
#define __TASK_H

#include <type.h>
#include <pmm.h>
#include <vmm.h>

typedef int32_t pid_t;

/* 进程状态 */
enum task_state {
    TASK_UNINIT = 0,    // 未初始化
    TASK_SLEEPING = 1,  // 睡眠
    TASK_RUNABLE = 2,   // 可运行 & 正在运行
    TASK_ZOMBIE = 3     // 僵死
};

/* kernel thread context */
/* 内核进程上下文 */
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

/* Process Control Block 
 * 进程控制块, 将会放在进程内核栈的最底端， 
 * 注意内核栈是由高至低生长的 */
struct proc_ctrl_blk {
    __volatile__ enum task_state state;
    pid_t pid;
    void *stack;
    struct mm_struct *mm;
    struct context context;     // context 
    struct proc_ctrl_blk *next;   // next task
};

extern pid_t now_pid;

/* int (*fun)(void *)这个语法是函数指针我竟然不知道  */
int32_t kernel_thread(int (*fn)(void *), void *arg);

void kthread_exit();

#endif
