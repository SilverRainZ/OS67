/* define the constant need by protect mode such as idt, gdt */
#ifndef __PM_H 
#define __PM_H 

struct gdt_entry{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

/* granularity = Flags Limit(16:19)
 * Flags = Gr Sz 0 0
 * Gr 等于1时， limit指定的内存粒度为4k, 界限可达 0xfffff * 4k = 4G
 * Sz = 0 16bit protect mode 
 *    = 1 32bit protect mode
 * */
struct gdt_ptr{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));


struct idt_entry{
    uint16_t base_low;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void gdt_set_gate(unsigned char num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void gdt_init();

void idt_init();
void idt_set_gate(unsigned char num, uint32_t base, uint16_t sel, uint8_t flags);
#endif
