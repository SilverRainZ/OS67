#include <sched.h>
#include <heap.h>
#include <debug.h>

struct proc_ctrl_blk *running_porc_head = NULL;
struct proc_ctrl_blk *wait_porc_head = NULL;
struct proc_ctrl_blk *current = NULL;

void sched_init(){
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
        switch_to(&(prev->next), &(current->next));
    }
}


