#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <type.h>
#include <isr.h>

#define NSYSCALL    21

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
#define SYS_SBRK    12
#define SYS_SLEEP   13
#define SYS_UPTIME  14
#define SYS_OPEN    15
#define SYS_WRITE   16
#define SYS_MKNOD   17
#define SYS_UNLINK  18
#define SYS_LINK    19
#define SYS_MKDIR   20
#define SYS_CLOSE   21

void sys_init();
void syscall();

#endif
