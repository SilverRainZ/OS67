/* str.c
 *
 * this file includes some library functions to process
 * string, they are simple, have not array bounds check.
 */
#include <lib.h>
#include <const.h>
int strlen(const char *str){
    int len;
    for(len = 0; *str; str++) len++;
    return len;
}

int strcmp(const char *str1, const char *str2){
    while (*str1||*str2){
        if (*str1 == *str2){
            str1++;
            str2++;
        }
        else return *str1 - *str2;
    }
    return 0;
}

char *strcpy(char *dest,const char *src){
    char *addr = dest;
    while (*src){
        *dest++ = *src++;
    }
    return addr;
}

char *strcat(char *dest,const char *src){
    char *addr = dest;
    while (*dest) dest++;
    do *dest++ = *src++; while (*src);   // end by zero
    return addr;
}

/* this function is case sensitive 
 * return the substring, not index*/
char *strstr(const char *str, const char *search){
    char *p = (char *)search;
    while (*str){
        while ((!*p)||*str == *p){
            if (!*p) return (char *)str;
            str++;
            p++;
        }
        p = (char *)search;
        str++;
    }
    return NULL;
}

