#ifndef __PRINTK_H
#define __PRINTK_H

// farseefc holy high!  :D
// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
// waiting for fixing, issue #2

void printk(const char *fmt, ...);
void _printl(const char *fmt, ...);

#ifdef __LOG_ON
#define printl(...) _printl(__VA_ARGS__)
#else
#define printl(...) ;
#endif

#endif 
