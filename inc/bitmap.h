#ifndef __BALLOC_H
#define __BALLOC_H

#include <type.h>
#include <minix.h>

int balloc(uint16_t dev);
void bfree(uint16_t dev, uint16_t blkno);
int _ialloc(uint16_t dev);
void _ifree(uint16_t dev, uint16_t ino);

#endif
