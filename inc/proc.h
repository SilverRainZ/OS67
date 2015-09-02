#ifndef __PROC_H
#define __PROC_H

#include <type.h>
#include <file.h>
#include <minix.h>

#define NPROC   128
#define NOFILE  128

/* proc state */
#define P_UNUSED    0x0
#define P_USED      0x1
#define P_RUNABLE   0x2
#define P_RUNNING   0x3
#define P_SLEEPING  0x4
#define P_ZOMBIE    0x5

struct context{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
};


struct proc{ 
    volatile uint8_t pid;
    uint32_t size;
    uint8_t state;
    uint8_t killed;
    char name[NAME_LEN];

    // context
    struct int_frame *fm;
    struct context *context;
    
    pde_t *pgdir;
    char *kern_stack;

    void *chan;

    struct file *ofile[NOFILE];
    struct inode *cwd;
    struct proc *parent;
};

extern struct proc *proc;
extern struct context *cpu_context;

void proc_init();
void scheduler();
void sched();
int fork();
int wait();
void sleep(void *chan);
int kill(uint8_t pid);
void exit();

#endif
