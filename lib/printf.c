/* printf.c
 * this file include a useful funcion printf()
 * suppose %d %x %c %s now
 */
#include <sys.h>
#include <lib.h>
#include <const.h>

typedef char *va_list;

/* integer to array , modify from
 * http://blog.csdn.net/wesley1991/article/details/5430350
 */
char* itoa(int value, char *str, int radix){
    char reverse[36];   
    char *p = reverse;
    bool sign = (value > 0)?TRUE:FALSE;
    value = (value > 0)?value:-value;
    *p++ = '\0';
    while (value > 0){
        *p++ = "0123456789abcdef"[value%radix];
        value /= radix;
    }
    if (!sign) *p = '-';
    else p--;
    while (p >= reverse){
        *str++ = *p--;
    }
    return str;
}

/* a simple printf, modify from
 * http://blog.tianya.cn/blogger/post_show.asp?BlogID=462085&PostID=8363874
 */
int vsprintf(char *buf, const char *fmt, va_list args){
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
                /* NB the pointer */
                itoa(*(int *)p_next_arg,p,10);
                p_next_arg += sizeof(int);
                p += strlen(p);
                break;
            case 'x':
                itoa(*(int *)p_next_arg,p,16);
                p_next_arg += sizeof(int);
                p += strlen(p);
                break;
            case 'c':
                *p++ = *(char *)p_next_arg;
                p_next_arg += sizeof(char);
                break;
            case 's':
                *p = '\0';
                p = strcat(p,*(char **)p_next_arg);
                p_next_arg += sizeof(char *);
                p += strlen(p);
                break;
            default:
                break;
        }
    }
    *p = '\0';
    return 0;
}
int printf(const char *fmt, ...){
    char buf[256];
    memset(buf, 0, sizeof(buf));
    va_list arg = (va_list)((char *)(&fmt) + sizeof(char *));
    vsprintf(buf, fmt, arg);
    puts(buf);
    return 0;
}

