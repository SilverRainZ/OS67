 /* idt.c 
  * This file is modified form Bram's Kernel Development Tutorial
  * install idt, 256 entrys table
  */

// std
#include <type.h>
// x86
#include <pm.h>
// libs
#include <string.h>

static struct idt_entry idt[NIDT];
struct idt_ptr idtp;
extern void idt_load();

void idt_install(uint8_t num, uint32_t base, uint16_t selctor, uint8_t gate, uint8_t flags){
    /* The interrupt routine's base address */
    idt[num].base_low = (base & 0xffff);
    idt[num].base_high = (base >> 16) & 0xffff;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idt[num].selector = selctor; 
    idt[num].always0 = 0; 
    idt[num].gate_type = 0x0f & gate;
    idt[num].flags = 0x0f & flags;
}

void idt_init(){
    idtp.limit = (sizeof (struct idt_entry) * NIDT) - 1;
    idtp.base = (uint32_t)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * NIDT);

    /* Add any new ISRs to the IDT here using idt_install */
    idt_load();
}
