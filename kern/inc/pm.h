/* define the constant need by protect mode such as idt, gdt */
#ifndef __PM_H 
#define __PM_H 

#define DA_32		0x4000	// 32 bit segment
#define DA_LIMIT_4K	0x8000	// Granularity is 4K 

// privilege / ring level
#define DA_DPL0		0x00
#define DA_DPL1		0x20
#define DA_DPL2		0x40
#define DA_DPL3		0x60

// descriptor type,all of them existsing 
#define DA_DR		0x90	// readable, data 
#define DA_DRW		0x92	// readable/writeable, data
#define DA_DRWA		0x93	// access readable/writeable, data 
#define DA_C		0x98	// runable, code
#define DA_CR		0x9a	// readable runable, code  
#define DA_CCO		0x9c	// runable, comfirm, code
#define DA_CCOR		0x9e	// runable, readable, comfirm, code

#define DA_LDT		0x82	
#define DA_TaskGate	0x85	
#define DA_386TSS	0x89
#define DA_386CGATE	0x8c	
#define DA_386IGATE	0x8e	
#define DA_386TGATE	0x8f	

// require privilege level
#define SA_RPL0		0	
#define SA_RPL1		1
#define SA_RPL2		2	
#define SA_RPL3		3	

#define SA_TIG		0
#define SA_TIL		4

#define PG_P		1	// page existing 
#define PG_RWR		0	// R/W readable/runable
#define PG_RWW		2	// R/W readable/writeable/runable
#define PG_USS		0	// U/S kernel 
#define PG_USU		4	// U/S user 

struct gdt_entry{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));


struct idt_entry{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));



#endif
