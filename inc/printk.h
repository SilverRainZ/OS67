#ifndef __PRINTK_H
#define __PRINTK_H

typedef char* va_list;

#define _INTSIZEOF(n) ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
/* (sizeof(n) + sizeof(int) - 1)%sizeof(int) * sizeof(int) */

#define va_start(ap, v)  (ap = (va_list)&v + _INTSIZEOF(v))
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap) (ap = (char*) 0)

void printk(const char *fmt, ...);
void _printl(const char *fmt, ...);
void _print_null(const char *fmt, ...);

// farseefc holy high!  :D
// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#ifdef __LOG_ON
#define printl(...) _printl(__VA_ARGS__)
#else
#define printl(...) _print_null(__VA_ARGS__)
#endif

#endif 
