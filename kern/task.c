/* task.h
 * This file is modified form hurlex 
 * 内核进程的建立, 无特权级转换
 */
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

/* 当前最大的PID */
pid_t now_pid = 0;

/* 内核线程创建 将PCB放在栈的最低地址处 */
int32_t kernel_thread(int (*fn)(void *), void *arg)
{
    struct task_struct *new_task = (struct task_struct *)kmalloc(STACK_SIZE);
    assert(new_task != NULL, "kern_thread: kmalloc error");

    /* PCB 初始化为0 */
    memset(new_task, 0,sizeof(struct task_struct));

    new_task->state = TASK_RUNNABLE;
    new_task->stack = current;
    new_task->pid = now_pid++;
    new_task->mm = NULL;

    uint32_t *stack_top = (uint32_t *)((uint32_t)new_task + STACK_SIZE);

    /* 构造堆栈 */
    *(--stack_top) = (uint32_t)arg;
    *(--stack_top) = (uint32_t)kthread_exit;
    *(--stack_top) = (uint32_t)fn;

    /* 设置正确的 ESP 指针*/
    new_task->context.esp = (uint32_t)new_task + STACK_SIZE - sizeof(uint32_t) * 3;

    /* 设置新任务的标志寄存器未屏蔽中断，很重要 */

    /* schedule() 是通过时钟中断触发的，为了防止中断重入
     * 中断发生时已经执行了cli(kern/loader.asm:167)
     * 因此在转到新进程后要开启中断(eflags的IF位)  */
    new_task->context.eflags = 0x200;
    new_task->next = running_proc_head;
    
    /* 找到当前进任务队列，插入到末尾 */
    struct task_struct *tail = running_proc_head;
    assert(tail != NULL, "Must init sched!");

    while (tail->next != running_proc_head) {
        tail = tail->next;
    }
    tail->next = new_task;

    return new_task->pid;
}

void kthread_exit()
{
    register uint32_t val asm ("eax");
    printk("Thread exited with value %d\n", val);
    while (1);
}

