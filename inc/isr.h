#ifndef __ISR_H
#define __ISR_H
/* isrs.c */
// this defines what the stack looks like after an ISR was running
struct regs_s{
    uint32_t gs, fs, es, ds;      
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  
    uint32_t int_no, err_code;    
    uint32_t eip, cs, eflags, useresp, ss;  
};    
void isrs_init();

/* irq.c */
void irq_install_handler(unsigned char irq, void (*handler)(struct regs_s *r));
void irq_uninstall_handler(unsigned char irq);
void irq_init();
#endif
