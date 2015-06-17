/* balloc.c
 * block alloc
 */
#include <type.h> 
#include <buf.h> 
#include <ide.h> 
#include <fs.h> 
#include <string.h> 
#include <printk.h> 
#include <dbg.h> 

void show_sb(){
    // TODO page fault here
    struct superblock sb;
    readsb(0, &sb);
    printk("sb ->size: %d ni: %d nb: %d\n", sb.size, sb.nblocks, sb.ninodes);
}

/* read the superblock */
void readsb(int dev, struct superblock *sb){
    struct buf *b;

    b = bread(dev, 1);
    char *tmp = (char *)sb;
    int i;
    // TODO replace with function memmove
    for (i = 0; i < 512; i++){
        tmp[i] = b->data[i];
    }
    brelse(b);
}


/* clear a block */
static void bzero(int dev, uint32_t blkn){
    struct buf *bp;
    bp = bread(0, blkn);
    memset(bp->data, 0, BSIZE);
    bwrite(bp);
    brelse(bp);
}

/* alloc a zeroed disk block*/
uint32_t balloc(int dev){
    int b, bi, m;
    struct buf *bp;
    struct superblock sb;
    
    bp = 0;
    readsb(dev,&sb);
    for (b = 0; b < sb.size; b += BPB){
        bp = bread(dev, BBLOCK(b, sb.ninodes));
        for(bi = 0; bi < BPB && b + bi < sb.size; bi++){
            m = 1 << (bi%8);
            if ((bp->data[bi/8] & m) == 0){ // This block is free
                bp->data[bi/8] |= m;
                bwrite(bp);                 // mark this block as uesed in bitmap
                bzero(dev, b + bi);         // clear block with zero
                return b + bi;
            }
        }
        brelse(bp);
    }
    panic("balloc: out of blocks");
    return ERROR;
}

void bfree(int dev, uint32_t blkn){
    struct buf *bp;
    struct superblock sb;
    int bi, m;

    readsb(dev, &sb);

    m = 1 << (blkn%8);
    bi = blkn/8;
    bp = bread(dev, BBLOCK(blkn, sb.ninodes));  // 找到该block对应的bitmap所在的block
    assert(bp->data[bi/8] & m,"bfree: freeing a free block")
    bp->data[bi/8] &= ~m;
    bwrite(bp);
    brelse(bp);
}

