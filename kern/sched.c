#include <sched.h>
#include <heap.h>
#include <dbg.h>

// 可调度进程链表
struct task_struct *running_proc_head = NULL;

// 等待进程链表
struct task_struct *wait_proc_head = NULL;

// 当前运行的任务
struct task_struct *current = NULL;

void init_sched()
{
	// 为当前执行流创建信息结构体 该结构位于当前执行流的栈最低端
	current = (struct task_struct *)(kern_stack_top - STACK_SIZE);

	current->state = TASK_RUNNABLE;
	current->pid = now_pid++;
	current->stack = current; 	// 该成员指向栈低地址
	current->mm = NULL; 		// 内核线程不需要该成员

	// 单向循环链表
	current->next = current;

	running_proc_head = current;
}

void schedule()
{
	if (current) {
		change_task_to(current->next);
	}
}

void change_task_to(struct task_struct *next)
{
	if (current != next) {
		struct task_struct *prev = current;
		current = next;
		switch_to(&(prev->context), &(current->context));
	}
}

