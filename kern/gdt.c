/* set the new gdt, the new gdt table has 256 entry */

#include <sys.h>
#include <pm.h>

struct gdt_entry gdt[256];  // we support 256 gdt entry
struct gdt_ptr gp;

extern void gdt_flush();    // extern func in loader.asm

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran){
    /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xffff);
    gdt[num].base_middle = (base >> 16) & 0xff;
    gdt[num].base_high = (base >> 24) & 0xff;

    /* Setup the descriptor limits */
    gdt[num].limit_low = (limit & 0xffff);
    gdt[num].granularity = ((limit >> 16) & 0x0f);

    /* Finally, set up the granularity and access flags */
    gdt[num].granularity |= (gran & 0xf0);
    gdt[num].access = access;
}

void gdt_init(){
    /* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct gdt_entry) * 256) - 1;
    gp.base = (unsigned int)&gdt;

    // null descriptor
    gdt_set_gate(0, 0, 0, 0, 0);  
    // type: code addr: 0 limit: 128M gran: 4KB bit 32bit 
    gdt_set_gate(1, 0, 0xffffffff, 0x9a, 0xcf);
    // type: data addr: 0 limit: 128M gran: 4KB bit 32bit 
    gdt_set_gate(2, 0, 0xffffffff, 0x92, 0xcf);

    int i;
    for (i = 3; i < 256; i++){
        gdt_set_gate(0, 0, 0, 0, 0);  
    }
    // clear gdt table 
    gdt_flush();
}
