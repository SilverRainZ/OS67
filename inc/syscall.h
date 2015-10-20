#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <type.h>
#include <isr.h>

#define NSYSCALL    20

// syscall nubmer
#define SYS_FORK    1
#define SYS_EXIT    2
#define SYS_WAIT    3
#define SYS_PIPE    4
#define SYS_READ    5
#define SYS_KILL    6
#define SYS_EXEC    7
#define SYS_FSTAT   8
#define SYS_CHDIR   9
#define SYS_FDUP    10
#define SYS_GETPID  11
#define SYS_SLEEP   12
#define SYS_UPTIME  13
#define SYS_OPEN    14
#define SYS_WRITE   15
#define SYS_MKNOD   16
#define SYS_UNLINK  17
#define SYS_LINK    18
#define SYS_MKDIR   19
#define SYS_CLOSE   20

int fetchint(uint32_t addr, int *ip);
int fetchstr(uint32_t addr, char **pp);
int argint(int n, int *ip);
int argstr(int n, char **pp);
int argptr(int n, char **pp, int size);

void sys_init();
void syscall();

#endif
