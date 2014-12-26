#ifndef __LIB_H
#define __LIB_H

#include <const.h>

/* std.c */

/* string.c */
void *memcpy(void *dest, const void *src, int count);
void *memset(void *dest, char val, int count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest,const char *src);
char *strcat(char *dest,const char *src);
char *strstr(const char *str, const char *search);

/* printf.c */
int printf(const char *fmt, ...);

/* queue.c */
#define QUEUE_MAX_LEN  256 
struct s_queue{
    /* we hava no malloc() now, 
     * so we use static array. */ 
    unsigned char base[QUEUE_MAX_LEN];
    int front;
    int rear;  
};
typedef struct s_queue queue;
typedef queue* pqueue;

bool queue_init(pqueue Q);
bool queue_en(pqueue Q, unsigned char e);
bool queue_de(pqueue Q, unsigned char *e);
int queue_getlen(pqueue Q);
bool queue_gethead(pqueue Q, unsigned char *e);

#endif
