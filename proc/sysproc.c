// #define __LOG_ON 1
// std
#include <type.h>
#include <timer.h>
#include <syscall.h>
// libs
#include <printk.h>
#include <string.h>
// proc
#include <proc.h>
#include <exec.h>
#include <sysproc.h>

int sys_fork(){
    return fork();
}

int sys_wait(){
    return wait();
}

int sys_exit(){
    printl("sys_exit: proc `%s`(PID: %d) exit\n", proc->name, proc->pid);
    exit();
    return 0; // no reach
}

int sys_exec(){
    char *path, *argv[MAX_ARGC];
    int i;
    uint32_t uargv, uarg; // pointer to argv[]

    if (argstr(0, &path) < 0 || argint(1, (int *)&uargv) < 0){
        return -1;
    }

    printl("sys_exec: get path %s argv: 0x%x\n", path, uargv);

    memset(argv, 0, sizeof(argv));

    for (i = 0; ; i++){
        if (i >= MAX_ARGC){
            return -1;  // too many arguments
        }
        if (fetchint(uargv + 4*i, (int *)&uarg) < 0){
            return -1;
        }
        if (uarg == 0){
            argv[i] = 0;
            break;
        }
        if (fetchstr(uarg, &argv[i]) < 0){
            return -1;
        }
    }

    return exec(path, argv);
}

int sys_kill(){
    int pid;

    if (argint(0, &pid) < 0){
        return -1;
    }

    printl("sys_kill: param: pid = %d\n", pid);
    return kill(pid);
}

int sys_getpid(){
    return proc->pid;
}

int sys_sleep(){
    int n, trick0;

    printl("sys_sleep: proc `%s`(PID: %d)\n", proc->name, proc->pid);
    if (argint(0, &n) < 0){
        return -1;
    }

    printl("sys_sleep: sleep for %d0ms\n", n);

    trick0 = timer_ticks;
    while ((int)timer_ticks - trick0 < n){

        printl("sys_sleep: proc `%s`(PID: %d) is still sleeping...\n", proc->name, proc->pid);

        if (proc->killed){
            return -1;
        }
        sleep(&timer_ticks);
    }

    printl("sys_sleep: proc `%s`(PID: %d) weakup, acutal time: %d0ms\n", proc->name, proc->pid, timer_ticks - trick0);
    return 0;
}

int sys_uptime(){
    return timer_ticks;
}
