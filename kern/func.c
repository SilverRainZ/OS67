/* some func */
#include <sys.h>

void *memcpy(void *dest, const void *src, int count){
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

void *memset(void *dest, char val, int count){
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count){
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

int strlen(const char *str){
    int len;
    for(len = 0; *str != '\0'; str++) len++;
    return len;
}

// asm instruction in/out
unsigned char inportb (unsigned short _port){
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb (unsigned short _port, unsigned char _data){
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}


