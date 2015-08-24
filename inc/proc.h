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

struct context{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
};


struct proc{ 
    volatile uint8_t pid;
    char name[NAME_LEN];
    uint32_t size;
    uint8_t state;

    struct int_frame *fm;
    struct context *context;
    
    char *kern_stack;
    pde_t *pgdir;

    struct file *ofile[NOFILE];
    struct inode *cwd;
    struct proc *parent;
};

extern struct proc *proc;

void proc_init();
void sched();

#endif
