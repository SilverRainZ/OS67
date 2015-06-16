/* balloc.c
 * block alloc
 */
#include <type.h> 
#include <buf.h> 
#include <ide.h> 
#include <fs.h> 
#include <printk.h> 
#include <dbg.h> 

/* read the superblock */
void readsb(int dev, struct superblock *sb){
    struct buf *b;

    b = bread(dev, 1);
    char *tmp = sb;
    int i;
    for (i = 0; i < 512){
        tmp[i] = b->data[i];
    }
    brelse(b);
}

/* clear a block */
static void bzero(int dev, uint32_t blkn){
    struct buf *bp;
    bp = readsp(0, blknum);
    memset(bp->data, 0, BSIZE);
    bwrite(bp);
    brelse(bp);
}

/* alloc a zeroed disk block*/
uint32_t balloc(int dev){

}

void bfree(int dev, uint32_t blkn){

}

