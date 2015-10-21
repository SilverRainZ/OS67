#ifndef __STRING_H
#define __STRING_H

#include <type.h>

void *memcpy(void *dest, const void *src, uint32_t count);
void *memset(void *dest, uint8_t val, uint32_t count);
uint16_t *memsetw(uint16_t *dest, uint16_t val, uint32_t count);

int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest,const char *src);
char *strcat(char *dest,const char *src);
char *strstr(const char *str, const char *search);

int strncmp(const char *str1, const char *str2, uint32_t n);
char *strncpy(char *dest, const char *src, uint32_t n);

#endif
