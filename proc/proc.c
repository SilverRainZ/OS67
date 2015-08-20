// std
#include <type.h>
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
#include <p2i.h>
// proc
#include <proc.h>

extern void _isr_stub_ret();

uint32_t cur_pid = 0;

void fork_ret(){
    return;
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
    return NULL;
}


void proc_userinit(){
    printl("proc_userinit: prepare for init\n");
    struct proc *pp;

    extern char __init_start;
    extern char __init_end;

    pp = proc_alloc();

    pp->pgdir = (pde_t *)pmm_alloc();
    printl("proc_userinit: get pgdir: 0x%x\n", pp->pgdir);


    kvm_init(pp->pgdir);
    vmm_map(pp->pgdir, (uint32_t)pp->kern_stack, (uint32_t)pp->kern_stack, PTE_P | PTE_R | PTE_K);
    printl("proc_userinit: kernel space maped\n");

    uint32_t size = &__init_end - &__init_start;
    pp->size = size;

    uvm_init_fst(pp->pgdir, &__init_start, size);
    printl("proc_userinit: user space maped\n");

    memset(pp->fm, 0, sizeof(*pp->fm));
    pp->fm->cs = SEL_USER_CODE | 0x3;
    pp->fm->ds = SEL_USER_DATA | 0x3;
    pp->fm->es = pp->fm->ds;
    pp->fm->fs = pp->fm->ds;
    pp->fm->gs = pp->fm->ds;
    pp->fm->ss = pp->fm->ds;
    pp->fm->eflags = FLAG_IF;
    pp->fm->user_esp = USER_BASE + PAGE_SIZE - 1;
    pp->fm->eip = USER_BASE;
    printl("proc_userinit: init stack build\n");

    strcpy(pp->name, "init");
    pp->cwd = path2inode("/");
    pp->state = P_RUNABLE;
    printl("proc_userinit: done, perpare for running\n");
}


