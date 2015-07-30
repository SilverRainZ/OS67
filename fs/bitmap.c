/* alloc.c
 * alloc/free data_block/inode
 */
#include <type.h> 
#include <buf.h> 
#include <ide.h> 
#include <minix.h>
#include <sb.h>
#include <bcache.h>
#include <bitmap.h> 
#include <string.h> 
#include <printk.h> 
#include <dbg.h> 


/* clear a block */
static void bzero(uint16_t dev, uint32_t blkno){
    struct buf *bp;

    bp = bread(dev, blkno);
    memset(bp->data, 0, BSIZE);
    bwrite(bp);
    brelse(bp);
}

/* alloc a zeroed data block  */
int balloc(uint16_t dev){
    uint32_t b, bi, m;
    struct buf *bp;
    struct super_block sb;
    
    //bp = 0;
    read_sb(dev,&sb);
    for (b = 0; b < sb.nzones; b += BPB){
        bp = bread(dev, ZMAP_BLK(sb, b));
        for(bi = 0; bi < BPB && b + bi < sb.nzones; bi++){
            m = 1 << (bi%8);
            if ((bp->data[bi/8] & m) == 0){ // This block is free
                bp->data[bi/8] |= m;
                bwrite(bp);                 // mark this block as uesed in bitmap
                brelse(bp);
                bzero(dev, b + bi);         // clear block with zero
                return sb.fst_data_zone + b + bi;
            }
        }
        brelse(bp);
    }
    panic("balloc: out of blocks");
    return ERROR;
}

void bfree(uint16_t dev, uint16_t blkno){
    struct buf *bp;
    struct super_block sb;
    uint32_t bi, m;

    read_sb(dev, &sb);

    assert(blkno >= sb.fst_data_zone ,"bfree: free a non-data block");
    assert(blkno < sb.nzones, "bfree: block number out of range");

    m = 1 << (blkno%8);
    bi = blkno/8;
    bp = bread(dev, ZMAP_BLK(sb, blkno - sb.fst_data_zone));  // 找到该block对应的bitmap所在的block

    assert(bp->data[bi/8] & m, "bfree: freeing a non-free block");

    bp->data[bi/8] &= ~m;
    bwrite(bp);
    brelse(bp);
}

/* alloc a *non-zeroed* inode 
 * NB: inode number is start at 1
 */
int _ialloc(uint16_t dev){
    uint32_t b, bi, m;
    struct buf *bp;
    struct super_block sb;

    read_sb(dev, &sb);

    for (b = 1; b < sb.ninodes; b += IPB){
        bp = bread(dev, IMAP_BLK(sb, b));
        for (bi = 0; bi < IPB && bi < sb.ninodes; bi++){
           m = 1 << (bi%8);
           if ((bp->data[bi/8] & m) == 0){
               bp->data[bi/8] |= m;
               bwrite(bp);
               brelse(bp);
               return b + bi;
           }
        }
    }
    panic("_ialloc: out of inode");
    return ERROR;
}

void _ifree(uint16_t dev, uint16_t ino){
    uint32_t bi, m;
    struct buf *bp;
    struct super_block sb;

    read_sb(dev, &sb);

    assert(ino > 0, "_ifree: inode-0 is no used");
    assert(ino < sb.ninodes, "_ifree: inode number out of range");

    bp = bread(dev, IMAP_BLK(sb, ino));
    bi = ino/8;
    m = 1 << (ino%8);

    assert(bp->data[bi] & m, "_ifree: free a non-free inode");
    bp->data[bi] &= ~m;
    bwrite(bp);
    brelse(bp);
}
