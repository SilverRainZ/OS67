// std
#include <type.h>
// mm
#include <pmm.h>
// proc
#include <proc.h>

static struct proc ptable[NPROC];
static uint32_t cur_pid = 0;

static struct proc *proc_alloc(){
    struct proc *pp;
    char *tos;

    for (pp = &ptable[0]; pp < &ptable[NPROC]; pp++){
        if (pp->state == P_UNUSED){
            pp->state = P_USED;
            pp->pid = ++cur_pid;

            pp->kern_stack = (char *)pmm_alloc();

            tos = pp->kern_stack + PAGE_SIZE;

        }
}
    
    return NULL;
}

void proc_init(){}


