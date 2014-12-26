 /* idt.c 
  * install idt, 256 entrys table, 
  * we use it to control interrupt */

#include <sys.h>
#include <pm.h>
#include <lib.h>

struct idt_entry idt[256];
struct idt_ptr idtp;
// exist in loader.asm
extern void idt_load();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags){
    /* The interrupt routine's base address */
    idt[num].base_lo = (base & 0xffff);
    idt[num].base_hi = (base >> 16) & 0xffff;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_init(){
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    /* Add any new ISRs to the IDT here using idt_set_gate */

    idt_load();
}
