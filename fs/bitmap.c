/* alloc.c
 * alloc/free data_block/inode
 */
// std
#define __LOG_ON 1
#include <type.h> 
#include <dbg.h> 
// libs
#include <string.h> 
#include <printk.h> 
// drv
#include <ide.h> 
// fs
#include <buf.h> 
#include <minix.h>
#include <sb.h>
#include <bcache.h>
#include <bitmap.h> 


/* clear a block */
static void bzero(uint16_t dev, uint32_t blkno){
    struct buf *bp;

    printl("bzero: zero blk-%d\n",blkno);

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
    uint32_t v_blkno;
    
    //bp = 0;
    read_sb(dev,&sb);
    for (b = 0; b < sb.nzones; b += BPB){
        bp = bread(dev, ZMAP_BLK(sb, b));
        for(bi = 0; bi < BPB && b + bi < sb.nzones; bi++){
            m = 1 << (bi%8);
            if ((bp->data[bi/8] & m) == 0){ // This block is free
                v_blkno = sb.fst_data_zone + b + bi - 1;
                printl("balloc: alloc blk-%d zmap blk-%d bi: %d\n", v_blkno, ZMAP_BLK(sb, b), bi);

                bp->data[bi/8] |= m;
                bwrite(bp);                 // mark this block as uesed in bitmap
                brelse(bp);
                bzero(dev, v_blkno);         // clear block with zero
                return v_blkno;
            }
        }
        brelse(bp);
    }
    panic("balloc: out of blocks");
    return -1;
}

void bfree(uint16_t dev, uint16_t blkno){
    struct buf *bp;
    struct super_block sb;
    uint32_t bi, m;

    read_sb(dev, &sb);

    assert(blkno >= sb.fst_data_zone ,"bfree: free a non-data block");
    assert(blkno < sb.nzones, "bfree: block number out of range");

    blkno -= sb.fst_data_zone - 1;

    bp = bread(dev, ZMAP_BLK(sb, blkno));  // 找到该block对应的bitmap所在的block

    bi = blkno%BPB;
    m = 1 << (bi%8);

    printl("bfree: free blk-%d zmapblk-%d bi: %d\n", blkno, ZMAP_BLK(sb, blkno - sb.fst_data_zone), bi);
    assert(bp->data[bi/8] & m, "bfree: freeing a free block");

    bp->data[bi/8] &= ~m;
    bwrite(bp);
    brelse(bp);
}

/* alloc a zeroed inode 
 * NB: inode number is start at 1
 */
int _ialloc(uint16_t dev){
    uint32_t b, bi, m;
    uint16_t ino;
    struct buf *bp;
    struct d_inode *d_ip;
    struct super_block sb;

    read_sb(dev, &sb);

    for (b = 1; b < sb.ninodes; b += IPB){
        bp = bread(dev, IMAP_BLK(sb, b));
        for (bi = 0; bi < IPB && bi < sb.ninodes; bi++){
           m = 1 << (bi%8);
           if ((bp->data[bi/8] & m) == 0){

               ino = b + bi - 1;
               printl("_ialloc: alloc inode-%d\n", ino);

               bp->data[bi/8] |= m;
               bwrite(bp);
               brelse(bp);


               /* fill inode with zero */
               bp = bread(dev, IBLK(sb,ino));
               d_ip = (struct d_inode *)bp->data + (ino - 1)%IPB;
               memset(d_ip, 0, sizeof(*d_ip)); bwrite(bp); brelse(bp);

               return ino;
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

    printl("_ifree: free inode-%d\n", ino);

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
