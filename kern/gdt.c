/* gdt.c 
 * This file is modified form Bram's Kernel Development Tutorial
 * set the new gdt, the new gdt table has 256 entrys
 */
// std
#include <type.h>
// x86
#include <pm.h>

static struct gdt_entry gdt[NGDT];  // we hava 256 gdt entry
struct gdt_ptr gp;

extern void gdt_flush();    // extern func in loader.asm

void gdt_install(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){

    /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xffff);
    gdt[num].base_middle = (base >> 16) & 0xff;
    gdt[num].base_high = (base >> 24) & 0xff;

    /* Setup the descriptor limits */
    gdt[num].limit_low = (limit & 0xffff);
    gdt[num].limit_high = ((limit >> 16) & 0x0f);

    /* Finally, set up the granularity and access flags */
    gdt[num].flags = 0x0f & flags;

    access |= AC_REVERSE;
    gdt[num].access = access;
}

void gdt_init(){
    /* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct gdt_entry) * NGDT) - 1;
    gp.base = (uint32_t)&gdt;

    /* null descriptor */
    gdt_install(0, 0, 0, 0, 0);  
    /* kernel code segment type: code addr: 0 limit: 4G gran: 4KB sz: 32bit */
    gdt_install(1, 0, 0xfffff, AC_RW|AC_EX|AC_PL_KERN|AC_PR, GDT_GR|GDT_SZ);
    /* kernel data segment type: data addr: 0 limit: 4G gran: 4KB sz: bit 32bit */
    gdt_install(2, 0, 0xfffff, AC_RW|AC_PL_KERN|AC_PR, GDT_GR|GDT_SZ); 
    /* user code segment type: data addr: 0 limit: 4G gran: 4KB sz: 32bit */
    gdt_install(3, 0, 0xfffff, AC_RW|AC_EX|AC_PL_USER|AC_PR, GDT_GR|GDT_SZ); 
    /* user code segment type: data addr: 0 limit: 4G gran: 4KB sz: 32bit */
    gdt_install(4, 0, 0xfffff, AC_RW|AC_PL_USER|AC_PR, GDT_GR|GDT_SZ); 
    
    gdt_flush();
}
