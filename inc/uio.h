#ifndef __UIO_H
#define __UIO_H

#include <type.h>

#define TTY     1

/* standard file descriptor */
#define stdin   0
#define stdout  1 
#define stderr  2

#define TTY_EOF     254
#define TTY_INT     255



int uio_init();

char getch();
char getchar();
int gets(char *str, uint32_t n);

void putch(int fd, char ch);
void putchar(char ch);
void puts(char *str);
void fprintf(int fd, char *fmt, ...);
void printf(char *fmt, ...);

#endif
