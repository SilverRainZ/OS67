/* irq.c 
 * This file is modified form Bram's Kernel Development Tutorial
 * Handle the Interrupt Requests(IRQs)h raised by hardware device 
 */
// std
#include <type.h>
#include <asm.h>
// x86
#include <pm.h>
#include <isr.h>

/* These are own ISRs that point to our special IRQ handler
*  instead of the regular 'fault_handler' function */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
void *irq_routines[ISR_NIRQ] ={
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* This installs a custom IRQ handler for the given IRQ */
void irq_install(uint8_t irq, void (*handler)(struct int_frame *r)){
    irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */
void irq_uninstall(uint8_t irq){
    irq_routines[irq] = 0;
}

/* in short: map irq 0-15 to int 32-47 */
void irq_remap(){
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

/* We first remap the interrupt controllers, and then we install
*  the appropriate ISRs to the correct entries in the IDT. This
*  is just like installing the exception handlers */
void irq_init(){
    irq_remap();

    idt_install(ISR_IRQ0 + 0, (uint32_t)irq0, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN); 
    idt_install(ISR_IRQ0 + 1, (uint32_t)irq1, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 2, (uint32_t)irq2, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 3, (uint32_t)irq3, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 4, (uint32_t)irq4, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 5, (uint32_t)irq5, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 6, (uint32_t)irq6, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 7, (uint32_t)irq7, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 8, (uint32_t)irq8, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 9, (uint32_t)irq9, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 10, (uint32_t)irq10, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 11, (uint32_t)irq11, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 12, (uint32_t)irq12, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 13, (uint32_t)irq13, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 14, (uint32_t)irq14, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
    idt_install(ISR_IRQ0 + 15, (uint32_t)irq15, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);
}

void irq_handler(struct int_frame *r){
    /* This is a blank function pointer */
    void (*handler)(struct int_frame *r);

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[r->int_no - ISR_IRQ0];
    if (handler){
        handler(r);
    }

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40){
        outb(0xA0, 0x20);
    }
    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outb(0x20, 0x20);
}
