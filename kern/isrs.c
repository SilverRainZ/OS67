/*set up the ISRs */

#include <sys.h>

/* int 0-31 is used to service exceptions! */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

/* This is a very repetitive function... it's not hard, it's
*  just annoying. As you can see, we set the first 32 entries
*  in the IDT to the first 32 ISRs. We can't use a for loop
*  for this, because there is no way to get the function names
*  that correspond to that given entry. We set the access
*  flags to 0x8E. This means that the entry is present, is
*  running in ring 0 (kernel level), and has the lower 5 bits
*  set to the required '14', which is represented by 'E' in
*  hex. */
void init_isrs()
{
    idt_set_gate(0, (unsigned long)(isr0 - 0x8000), 0x08, 0x8E);
    idt_set_gate(1, (unsigned long)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned long)isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned long)isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned long)isr4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned long)isr5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned long)isr6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned long)isr7, 0x08, 0x8E);

    idt_set_gate(8, (unsigned long)isr8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned long)isr9, 0x08, 0x8E);
    idt_set_gate(10, (unsigned long)isr10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned long)isr11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned long)isr12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned long)isr13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned long)isr14, 0x08, 0x8E);
    idt_set_gate(15, (unsigned long)isr15, 0x08, 0x8E);

    idt_set_gate(16, (unsigned long)isr16, 0x08, 0x8E);
    idt_set_gate(17, (unsigned long)isr17, 0x08, 0x8E);
    idt_set_gate(18, (unsigned long)isr18, 0x08, 0x8E);
    idt_set_gate(19, (unsigned long)isr19, 0x08, 0x8E);
    idt_set_gate(20, (unsigned long)isr20, 0x08, 0x8E);
    idt_set_gate(21, (unsigned long)isr21, 0x08, 0x8E);
    idt_set_gate(22, (unsigned long)isr22, 0x08, 0x8E);
    idt_set_gate(23, (unsigned long)isr23, 0x08, 0x8E);

    idt_set_gate(24, (unsigned long)isr24, 0x08, 0x8E);
    idt_set_gate(25, (unsigned long)isr25, 0x08, 0x8E);
    idt_set_gate(26, (unsigned long)isr26, 0x08, 0x8E);
    idt_set_gate(27, (unsigned long)isr27, 0x08, 0x8E);
    idt_set_gate(28, (unsigned long)isr28, 0x08, 0x8E);
    idt_set_gate(29, (unsigned long)isr29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned long)isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned long)isr31, 0x08, 0x8E);
}

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
char *exception_messages[] = {
    "Division By Zero\0",
    "Debug\0",
    "Non Maskable Interrupt\0",
    "Breakpoint\0",
    "Into Detected Overflow\0",
    "Out of Bounds\0",
    "Invalid Opcode\0",
    "No Coprocessor\0",

    "Double Fault\0",
    "Coprocessor Segment Overrun\0",
    "Bad TSS\0",
    "Segment Not Present\0",
    "Stack Fault\0",
    "General Protection Fault\0",
    "Page Fault\0",
    "Unknown Interrupt\0",

    "Coprocessor Fault\0",
    "Alignment Check\0",
    "Machine Check\0",
    "Reserved\0",
    "Reserved\0",
    "Reserved\0",
    "Reserved\0",
    "Reserved\0",

    "Reserved\0",
    "Reserved\0",
    "Reserved\0",
    "Reserved\0",
    "Reserved\0",
    "Reserved\0",
    "Reserved\0",
    "Reserved\0"
};

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void fault_handler(struct regs *r)
{
    if (r->int_no < 32)
    {
        setcolor(COL_RED, COL_BLACK);
        puts((unsigned char *)exception_messages[r->int_no]);
        puts((unsigned char *)" Exception.\n\rSystem Halted!\n\r\0");
        for (;;);
    }
}
