#ifndef __BCACHE_H
#define __BCACHE_H

#include <buf.h>

#define NBUF 128


void bcache_init();
struct buf* bread(char dev, uint32_t blkno);
void bwrite(struct buf* b);
void brelse(struct buf *b);

#endif
