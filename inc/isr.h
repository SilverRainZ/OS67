#ifndef __ISR_H
#define __ISR_H
/* isrs.c */

// this defines what the stack looks like after an ISR was running

#include <type.h>

#define ISR_IRQ0    32
#define ISR_NIRQ    16
#define ISR_UNKNOWN 255

struct regs_s{
    uint32_t gs, fs, es, ds;      
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  
    uint32_t int_no, err_code;    
    uint32_t eip, cs, eflags, user_esp, ss;  
};    

/* irq.c */
void isr_init();

/* irq.c */
#define IRQ_TIMER   0
#define IRQ_KB      1
#define IRQ_IDE     14

void irq_handler(struct regs_s *r);
void irq_install(unsigned char irq, void (*handler)(struct regs_s *r));
void irq_uninstall(unsigned char irq);
void irq_init();

/* fault.c */
#define FAULT_PAGE  14
void fault_init();
void fault_handler(struct regs_s *r);

#endif
