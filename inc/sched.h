#ifndef __SCHED_H
#define __SCHED_H

#include <task.h>

/* 可调度进程链表 */
extern struct proc_ctrl_blk *running_porc_head;
/* 正在等待的进程列表 */
extern struct proc_ctrl_blk *wait_porc_head;
/* 当前进程 */
extern struct proc_ctrl_blk *current;

void sched_init();
/* 调度函数 */
void schedule();
void change_task_to(struct proc_ctrl_blk *next);
/* 通过构造栈切换任务， 函数在loader.asm中导出 */
void switch_to(struct context *prev, struct context *next);

#endif

