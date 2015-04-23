/* printk.c
 * this file include a useful funcion printk()
 * suppose %d %x %c %s now
 */
//TODO %f doesn't work now, don't use it
//#define __DEBUG

#ifdef __DEBUG
#include <stdio.h>
#include <string.h>
#undef NULL

/* TEST */
#include "../inc/type.h"
#include "../inc/printk.h"
int main(){
    printk("big int test: %x\n", 0xbaadf00d);
    printk("double test: %f and %f\n",305.53, 123.45);
    return 0;
}

#else
#include <type.h>
#include <vga.h>
#include <string.h>
#include <printk.h>
#endif

char* itoa(int value, char *str, int radix){
    char reverse[36];   
    char *p = reverse;
    bool sign = (value >= 0)?TRUE:FALSE;
    value = (value >= 0)?value:-value;
    *p++ = '\0';
    while (value >= 0){
        *p++ = "0123456789abcdef"[value%radix];
        value /= radix;
        if (value == 0) break;
    }
    if (!sign) *p = '-';
    else p--;
    while (p >= reverse){
        *str++ = *p--;
    }
    return str;
}

char* uitoa(unsigned int value, char *str, int radix){
    char reverse[36];   
    char *p = reverse;
    *p++ = '\0';
    while (value >= 0){
        *p++ = "0123456789abcdef"[value%radix];
        value /= radix;
        if (value == 0) break;
    }
    p--;
    while (p >= reverse){
        *str++ = *p--;
    }
    return str;
}
char* gcvt(double value, int ndigit, char *buf){
    char tmpbuf[72];
    int int_part = (int)value;
    printk("int: %d\n", int_part);
    double folat_part = value - int_part;
    if (folat_part < 0) folat_part = -folat_part;
    itoa(int_part, tmpbuf, 10);
    char *p = tmpbuf;
    printk("tmpbuf: %s\n", tmpbuf);

     while (*p != '\0') p++;
    *p++ = '.';
    while (ndigit > 0 && folat_part > 0.00000001){
        *p++ = (int)(folat_part*10) + '0';
        folat_part = (folat_part * 10) - (int)(folat_part * 10);
        ndigit--;
    }
    *p = '\0';
    printk("tmpbuf2: %s\n", tmpbuf);
    strcpy(buf, tmpbuf);
    return buf;
}

void vsprintk(char *buf, const char *fmt, va_list args){
    char *p;
    va_list p_next_arg = args;
    for (p = buf; *fmt; fmt++){
        if (*fmt != '%'){
            *p++ = *fmt;
            continue;
        }
        fmt++;  // *fmt = '%'
        switch (*fmt){
            case 'd':
                itoa(va_arg(p_next_arg, int),p,10);
                p += strlen(p);
                break;
            case 'x':
                uitoa(va_arg(p_next_arg, unsigned int),p,16);
                p += strlen(p);
                break;
            case 'c':
                *p++ = va_arg(p_next_arg, char);
                break;
            case 's':
                *p = '\0';
                strcat(p, va_arg(p_next_arg, char *));
                p += strlen(p);
                break;
            case 'f':
                gcvt(va_arg(p_next_arg, double), 6, p);
                p += strlen(p);
                break;
            default:
                break;
        }
    }
    *p = '\0';
}
void printk(const char *fmt, ...){
    //使用static会给调试带来麻烦
    char buf[256];
    memset(buf, 0, sizeof(buf));
    va_list args;
    va_start(args, fmt);
    vsprintk(buf, fmt, args);
    va_end(args);
    puts(buf);
}

