#include <type.h>
#include <pm.h>
#include <pmm.h>
#include <vmm.h>
#include <heap.h>
#include <dbg.h>
#include <string.h>
#include <sched.h>
#include <task.h>
#include <printk.h>

pid_t now_pid = 0;

/* 创建内核进程 */
int32_t kernel_thread(int (*fn)(void *), void *arg){
    /* 为新进程申请栈 */
    struct proc_ctrl_blk *new_task = (struct proc_ctrl_blk *)kmalloc(STACK_SIZE);
    assert(new_task != NULL ,"kernel_thread malloc fault");

    memset(new_task, 0, sizeof(struct proc_ctrl_blk));
    new_task->state  = TASK_RUNABLE;
    new_task->stack = current;  //TODO
    new_task->pid = now_pid++;
    new_task->mm = NULL;
    
    uint32_t *stack_top = (uint32_t *)((uint32_t)new_task + STACK_SIZE);

    // 压入这三个？
    *(--stack_top) = (uint32_t)arg;
    *(--stack_top) = (uint32_t)kthread_exit;
    *(--stack_top) = (uint32_t)fn;

    new_task->context.esp = (uint32_t)new_task + STACK_SIZE - sizeof(uint32_t)*3;
    /* 新进程不屏蔽中断  因为要通过时钟中断切换任务 */
    new_task->context.eflags = 0x200; // block interrupt

    /* 将自分插入running_porc_head循环链表的尾部 */
    new_task->next = running_porc_head;
    struct proc_ctrl_blk *tail = running_porc_head;
    assert(tail != NULL, "Must init sched");

    while (tail->next != running_porc_head){
        tail = tail->next;
    }
    tail->next = new_task;

    return new_task->pid;
}

void kthread_exit(){
    register uint32_t val asm ("eax");
    printk("thread exit with value %d\n", val);
    while (1);
}
