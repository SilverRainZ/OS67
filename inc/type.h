#ifndef __CONST_H
#define __CONST_H

typedef char bool;
#define TRUE    1
#define FALSE   0
#define OK      0
#define ERROR  -1 
#define NULL (void *)0

/* std int */
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef char int8_t;

/* useful macro */
#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)>(b)?(a):(b))

#endif
