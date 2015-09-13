#ifndef __SYSPROC_H
#define __SYSPROC_H

int sys_fork();
int sys_wait();
int sys_exit();
int sys_kill();
int sys_exec();
int sys_getpid();
int sys_sleep();
int sys_uptime();

#endif
