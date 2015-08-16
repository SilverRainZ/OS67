#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <type.h>
#include <isr.h>

#define ISR_SYSCALL  0x80

void sys_init();
void syscall();

#endif
