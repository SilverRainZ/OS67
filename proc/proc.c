// #define __LOG_ON 1

// std
#include <type.h>
#include <dbg.h>
#include <asm.h>
// x86
#include <pm.h>
#include <isr.h>
// libs
#include <string.h>
#include <printk.h>
// mm
#include <pmm.h>
#include <vmm.h>
// fs
#include <inode.h>
#include <p2i.h>
// proc
#include <proc.h>

extern void _isr_stub_ret();
extern void context_switch(struct context **old, struct context *new);


static uint32_t cur_pid = 0;
static struct proc *initproc = NULL;
static struct proc ptable[NPROC];
struct context *cpu_context;

struct proc *proc = NULL;

/* import pic_init(), only use in fork_ret */
extern void pic_init();
/* do not ask me why call pic_init here, irq didn't work after fork()
 * I have to reinit it
 * thx to fleuria
 */
void fork_ret(){
    static int fst = 1;

    if (fst == 1){
        fst = 0;
    } else {
        pic_init(); 
    }

    return;
}

void print_proc(struct proc *pp){
    uint32_t pa;

    vmm_get_mapping(pp->pgdir, USER_BASE, &pa);

    printl("print_proc:\n");
    printl("    name: %s\n",pp->name);
    printk("    pid: %d\n", pp->pid);
    printl("    kern_stack: 0x%x\n",pp->kern_stack);
    printl("    pgdir: 0x%x\n",pp->pgdir);
    printl("    phy addr: 0x%x\n", pa);
}

static struct proc *proc_alloc(){
    struct proc *pp;
    char *tos;

    for (pp = &ptable[0]; pp < &ptable[NPROC]; pp++){
        /* found */
        if (pp->state == P_UNUSED){
            printl("proc_alloc: found a unused entry\n");

            pp->state = P_USED;
            pp->pid = ++cur_pid;

            /* alloc kernstack */
            pp->kern_stack = (char *)pmm_alloc();
            tos = pp->kern_stack + PAGE_SIZE;

            tos -= sizeof(*pp->fm);
            pp->fm = (struct int_frame *)tos;

            tos -= 4;
            *(uint32_t *)tos = (uint32_t)_isr_stub_ret;

            tos -= sizeof(*pp->context);
            pp->context = (struct context *)tos;
            memset(pp->context, 0, sizeof(*pp->context));

            pp->context->eip = (uint32_t)fork_ret;

            return pp;
        }
    }
    return 0;
}

void proc_init(){
    struct proc *pp;
    extern char __init_start;
    extern char __init_end;

    cur_pid = 0;

    printl("proc_userinit: clear ptable\n");

    memset(ptable, 0, sizeof(struct proc) * NPROC);

    printl("proc_userinit: prepare for init\n");

    pp = proc_alloc();

    pp->pgdir = (pde_t *)pmm_alloc();
    printl("proc_userinit: get pgdir: 0x%x\n", pp->pgdir);

    kvm_init(pp->pgdir);
    vmm_map(pp->pgdir, (uint32_t)pp->kern_stack, (uint32_t)pp->kern_stack, PTE_P | PTE_R | PTE_K);
    printl("proc_userinit: kernel space maped\n");

    uint32_t size = &__init_end - &__init_start;
    pp->size = PAGE_SIZE;

    uvm_init_fst(pp->pgdir, &__init_start, size);
    printl("proc_serinit: user space maped\n");

    memset(pp->fm, 0, sizeof(*pp->fm));
    pp->fm->cs = (SEL_UCODE << 3) | 0x3;
    pp->fm->ds = (SEL_UDATA << 3) | 0x3;
    pp->fm->es = pp->fm->ds;
    pp->fm->fs = pp->fm->ds;
    pp->fm->gs = pp->fm->ds;
    pp->fm->ss = pp->fm->ds;
    pp->fm->eflags = FLAG_IF;
    pp->fm->user_esp = USER_BASE + PAGE_SIZE;
    pp->fm->eip = USER_BASE;
    printl("proc_init: init stack build\n");

    strcpy(pp->name, "init");

    pp->cwd = p2i("/");
    pp->state = P_RUNABLE;

    initproc = pp;

    printl("proc_init: done, perpare for running\n");
}


void scheduler(){
    struct proc *pp;

    printl("scheduler: start\n");

    for (;;){
        for (pp = &ptable[0]; pp < &ptable[NPROC]; pp++){
            cli();  // 不清楚此处是否会有死锁或冲突的风险， 反正关中断也不要钱
            if (pp->state != P_RUNABLE){
                continue;
            }

            printl("scheduler: proc `%s`(PID: %d) will run\n", pp->name, pp->pid);

            uvm_switch(pp);
            pp->state = P_RUNNING;

            proc = pp;

            printl(">>>> context switch\n");
            context_switch(&cpu_context, pp->context);
            printl("<<<< return form proc `%s`(PID: %d)\n", pp->name, pp->pid);
            sti();
        }
    }
}

void sched(){
    assert(proc->state != P_RUNABLE, "sched: no runable")

    if (proc->state == P_RUNNING){
        proc->state = P_RUNABLE;
    }
    context_switch(&proc->context, cpu_context);
}

void sleep(void *chan){
    assert(proc, "sleep: no proc");

    printl("sleep: proc `%s`(PID: %d) is going to sleep...\n", proc->name, proc->pid);
    /* go to sleep
     * NB: This operation MUST be atomic
     * 这是一处确定有死锁风险的代码，当 chan 赋值后，如果父进程的 wait 马上执行，
     * 那么 wait 将因为找不到任何需要唤醒的进程而退出，此后本进程才进入 P_SLEEPING 状态。 
     * 只是这时已经没有进程能够唤醒它了。
     */
    cli();
    proc->chan = chan;
    proc->state = P_SLEEPING;
    sti();

    sched();

    // wake up
    proc->chan = 0;

    printl("sleep: proc `%s`(PID: %d)  wakeup...\n", proc->name, proc->pid);

    // yes, we call pic_init again... :(
    pic_init();
}

void wakeup(void *chan){
    struct proc *pp;

    for (pp = ptable; pp < &ptable[NPROC]; pp++){
        if (pp->state == P_SLEEPING && pp->chan == chan){
            pp->state = P_RUNABLE;
        }
    }
}

int wait(){
    uint8_t havekids, pid;
    struct proc* pp;

    printl("wait: waiting...\n");
    for (;;){
        havekids = 0;
        for (pp = ptable; pp <= &ptable[NPROC]; pp++){
            if (pp->parent != proc){
                continue;
            }

            havekids = 1;
            
            if (pp->state == P_ZOMBIE){
                printl("wait: recycle proc `%s`(PID: %d)\n", pp->name, pp->pid);
                // can be clear
                pid = pp->pid;

                /* free mem */
                pmm_free((uint32_t)pp->kern_stack);
                pp->kern_stack = 0;
                uvm_free(pp->pgdir);

                pp->state = P_UNUSED;
                pp->pid = 0;
                pp->parent = 0;
                pp->name[0] = 0;
                pp->killed = 0;

                return pid;
            }
        }

        if (!havekids || proc->killed){
            return -1;
        }

        // wait for chidren to exit
        sleep(proc);
    }
}

void exit(){
    struct proc *pp;
    int fd;

    assert(proc != initproc, "exit: initproc can no exit");

    printl("exit: closing opening file\n");
    for (fd = 0; fd < NOFILE; fd++){
        if (proc->ofile[fd]){
            fclose(proc->ofile[fd]);
            proc->ofile[fd] = 0;
        }
    }

    iput(proc->cwd);
    proc->cwd = 0;

    cli();
    //wakeup(proc->parent);

    printl("exit: collecting subprocess\n");
    for (pp = ptable; pp < &proc[NPROC]; pp++){
        if (pp->parent == proc){
            pp->parent = proc->parent;
            if (pp->state == P_ZOMBIE){
                wakeup(proc->parent);
            }
        }
    }
    wakeup(proc->parent);

    printl("exit: ZOMBIE\n");
    proc->state = P_ZOMBIE;
    sti();

    sched();
    panic("exit: return form sched");
}

int kill(uint8_t pid){
    struct proc *pp;

    for (pp = ptable; pp < &ptable[NPROC]; pp++){
        if (pp->pid == pid){
            pp->killed = 1;

            if (pp->state == P_SLEEPING){
                pp->state = P_RUNABLE;
            }
            return 0;
        }
    }
    return -1;
}

int fork(){
    int i;
    struct proc *child;

    printl("fork: fork `%s`\n", proc->name);

    if ((child = proc_alloc()) == 0){
        return -1;
    }

    printl("fork: copying memory...\n");

    child->pgdir = uvm_copy(proc->pgdir, proc->size);

    if (child->pgdir == 0){
        panic("fork:");
        pmm_free((uint32_t)child->kern_stack);
        child->kern_stack = 0;
        child->state = P_UNUSED;
        return -1;
    }

    printl("fork: copying attrib...\n");
    child->size = proc->size;
    child->parent = proc;
    *child->fm = *proc->fm; // return from same address

    child->fm->eax = 0;
    printl("fork: dup opened files\n");
    for (i = 0; i < NOFILE; i++){
        if (proc->ofile[i]){
            child->ofile[i] = fdup(proc->ofile[i]);
        }
    }

    child->cwd = idup(proc->cwd);
    strncpy(child->name, proc->name, sizeof(proc->name));

    child->state = P_RUNABLE;

    printl("fork: done\n");
    return child->pid;
}
