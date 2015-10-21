// #define __LOG_ON 1

//std
#include <type.h>
#include <asm.h>
// libs
#include <printk.h>
// x86
#include <pm.h>
#include <isr.h>
#include <syscall.h>
// mm
#include <vmm.h>
// fs
#include <sysfile.h>
// proc
#include <proc.h>
#include <sysproc.h>

extern void _syscall();

int sys_none();
static int (*sys_routines[])(void) = {
    sys_none,
    sys_fork,
    sys_exit,
    sys_wait,
    sys_pipe,
    sys_read,
    sys_kill,
    sys_exec,
    sys_fstat,
    sys_chdir,
    sys_dup,
    sys_getpid,
    sys_sleep,
    sys_uptime,
    sys_open,
    sys_write,
    sys_mknod,
    sys_unlink,
    sys_link,
    sys_mkdir,
    sys_close
}; 

/* syscall name for debug */
char *sys_name[NSYSCALL + 1] = {
    "sys_none",
    "sys_fork",
    "sys_exit",
    "sys_wait",
    "sys_pipe",
    "sys_read",
    "sys_kill",
    "sys_exec",
    "sys_fstat",
    "sys_chdir",
    "sys_dup",
    "sys_getpid",
    "sys_sleep",
    "sys_uptime",
    "sys_open",
    "sys_write",
    "sys_mknod",
    "sys_unlink",
    "sys_link",
    "sys_mkdir",
    "sys_close"
};

int fetchint(uint32_t addr, int *ip){
    if (addr < USER_BASE 
            || addr > USER_BASE + proc->size 
            || addr + 4 > USER_BASE + proc->size){
        printl("fetchint: failed, addr 0x%x\n", addr);
        return -1;
    }
    *ip = *(int *)(addr);

    return 0;
}

int fetchstr(uint32_t addr, char **pp){
    char *s, *ep;

    if (addr < USER_BASE || addr >= USER_BASE + proc->size){
        printl("fetchstr: failed, addr 0x%x\n", addr);
        return -1;
    }

    *pp = (char *)addr;
    ep = (char *)(USER_BASE + proc->size);

    for (s = *pp; s < ep; s++){
        if (*s == '\0'){
            return s - *pp;
        }
    }

    return -1;
}

int argint(int n, int *ip){
    return fetchint(proc->fm->user_esp + 4*n + 4, ip);  // "+4" for eip
}

int argstr(int n, char **pp){
    int addr;

    if (argint(n, &addr) < 0){
        return -1;
    }
    
    return fetchstr((uint32_t)addr, pp);
}

int argptr(int n, char **pp, int size){
    int addr;

    if(argint(n, &addr) < 0){
        return -1;
    }

    if ((uint32_t)addr < USER_BASE 
            || (uint32_t)addr >= USER_BASE + proc->size 
            || (uint32_t)addr + size >= USER_BASE + proc->size){
        return -1;
    }

    *pp = (char *)addr;

    return 0;
}

int sys_none(){
    return -1;
}

void sys_init(){
    idt_install(ISR_SYSCALL, (uint32_t)_syscall, SEL_KCODE << 3, GATE_TRAP, IDT_PR | IDT_DPL_USER);
}

void syscall(){
    int cn;

    printl(">>>>>>> syscall %s: number: %d, form `%s`(PID: %d)\n", sys_name[proc->fm->eax], proc->fm->eax, proc->name, proc->pid);

    cn = proc->fm->eax;

    if (cn > 0 && cn <= NSYSCALL && sys_routines[cn]){
        proc->fm->eax = sys_routines[cn]();
    } else {
        printl("syscall: no such syscall\n");
        proc->fm->eax = -1;
    }
}
