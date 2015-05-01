#ifdef __SCHED_H
#define __SCHED_H

#include <task.h>

/* process chain can be sched */
extern struct proc_ctrl_blk *running_porc_head;
/* */
extern struct proc_ctrl_blk *wait_porc_head;
extern struct proc_ctrl_blk *current;

void sched_init();
void schedule();
void change_task_to(struct proc_ctrl_blk *next);
void switch_to(struct context *prev, struct context *next);

#endif __SCHED_H

