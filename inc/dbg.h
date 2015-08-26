#ifndef __DBG_C
#define __DBG_C

#define assert(x, info)                         \
    if (!(x)){                                  \
        printk("In file: %s\n", __FILE__);      \
        printk("In line: %d\n", __LINE__);      \
        printk("In func: %s\n", __FUNCTION__);  \
        panic(info);                            \
    }                                       

void panic(const char *msg);
void print_cur_status();
void print_stack_trace();

/********* Debug functions which depend on bochs ****************/
void bochs_putc(char c);

/* stops simulation and breaks into the debug console */
#define bochs_break()       \
    outw(0x8A00,0x8A00);    \
    outw(0x8A00,0x08AE0);   

#endif
