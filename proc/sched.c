// std
#include <type.h>
#include <asm.h>
#include <dbg.h>
// libs
#include <printk.h>
// mm
#include <vmm.h>
// proc
#include <proc.h>
#include <sched.h>

extern void context_switch(struct context **old, struct context *new);
struct context *con;
struct context con1;

void sched(){
    struct proc *pp;
    printl("sched:\n");
    con = &con1;
    for (;;){
        for (pp = &ptable[0]; pp < &ptable[NPROC]; pp++){
            if (pp->state != P_RUNABLE){
                continue;
            }
            printl("sched: found\n");
            uvm_switch(pp);
            pp->state = P_RUNNING;

            printl("sched: context_switch\n");
            context_switch(&con, pp->context);
        }
    }
}
