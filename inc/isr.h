#ifndef __ISR_H
#define __ISR_H
/* isrs.c */
// this defines what the stack looks like after an ISR was running
struct regs{
    unsigned int gs, fs, es, ds;      
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  
    unsigned int int_no, err_code;    
    unsigned int eip, cs, eflags, useresp, ss;  
};    
void isrs_init();

/* irq.c */
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);
void irq_init();
#endif
