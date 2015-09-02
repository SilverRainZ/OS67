// std
#include <type.h>
#include <timer.h>
#include <syscall.h>
// proc
#include <proc.h>
#include <sysproc.h>

int sys_fork(){
    return fork();
}

int sys_wait(){
    return wait();
}

int sys_exit(){
    exit();
    return 0; // no reach
}

int sys_kill(){
    int pid;

    if (argint(0, &pid) < 0){
        return -1;
    }
    return kill(pid);
}

int sys_getpid(){
    return proc->pid;
}

int sys_sleep(){
    return 0;
}

int sys_uptime(){
    return timer_ticks;
}
