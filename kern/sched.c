#include <sched.h>
#include <heap.h>
#include <dbg.h>

/* 可调度进程链表 */
struct proc_ctrl_blk *running_porc_head = NULL;
/* 正在等待的进程链表 */
struct proc_ctrl_blk *wait_porc_head = NULL;
/* 当前进程 */
struct proc_ctrl_blk *current = NULL;

void sched_init(){
    /* 为当前执行流（osmain）创建PCB，位于栈的最底处 */
    current = (struct proc_ctrl_blk *)(kern_stack_top - STACK_SIZE);

    current->state = TASK_RUNABLE;
    current->pid = now_pid++;
    current->stack = current;
    current->mm = NULL;

    current->next = current;
    running_porc_head = current;
}

void schedule(){
    if (current){
        change_task_to(current->next);
    }
}

void change_task_to(struct proc_ctrl_blk *next){
    if (current != next){
        struct proc_ctrl_blk *prev = current;
        current = next;
        switch_to(&(prev->context), &(current->context));
    }
}


