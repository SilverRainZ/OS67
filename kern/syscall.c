//std
#include <type.h>
// libs
#include <printk.h>
// x86
#include <pm.h>
#include <isr.h>
#include <syscall.h>

extern void _syscall();

void sys_init(){
    idt_install(ISR_SYSCALL, (uint32_t)_syscall, SEL_KERN_CODE, GATE_TRAP, IDT_PR | IDT_DPL_USER);
}

void syscall(struct int_frame *r){
    printk("SYSCALL: %d\n", r->int_no);
    for (;;);
}
