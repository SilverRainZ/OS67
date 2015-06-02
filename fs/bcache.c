/* buffer cache 
 * This file is modified form xv6
 */
#include <type.h>
#include <buf.h>

struct bcache{
    struct buf buf[NBUF];
};

void bcache_init(){

}

static struct buf* bget(char dev, uint32_t lba){

}

static struct buf* bread(char dev, uint32_t lba){

}

void bwrite(struct buf* b){
    
}

void brelse(struct buf *b){

}
