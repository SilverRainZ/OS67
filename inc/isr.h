#ifndef __ISR_H
#define __ISR_H
/* isr.c */

// this defines what the stack looks like after an ISR was running

#include <type.h>

#define ISR_IRQ0    32
#define ISR_NIRQ    16
#define ISR_SYSCALL 0x80
#define ISR_UNKNOWN 255

struct int_frame{
    /* segment registers */
    uint32_t gs;    // 16 bits
    uint32_t fs;    // 16 bits
    uint32_t es;    // 16 bits
    uint32_t ds;          // 16 bits

    /* registers save by pusha */
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t int_no;

    /* save by `int` instruction */
    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;    // 16 bits
    uint32_t eflags;
    uint32_t user_esp;
    uint32_t ss;    // 16 bits
};    

/* isr.c */
void isr_init();

/* irq.c */
#define IRQ_TIMER   0
#define IRQ_KB      1
#define IRQ_IDE     14

void irq_handler(struct int_frame *r);
void irq_install(uint8_t irq, void (*handler)(struct int_frame *r));
void irq_uninstall(uint8_t irq);
void irq_init();

/* fault.c */
#define FAULT_PAGE  14

void fault_init();
void fault_handler(struct int_frame *r);

#endif
