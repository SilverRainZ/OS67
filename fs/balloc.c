/* balloc.c
 * block alloc
 */
#include <type.h> 
#include <buf.h> 
#include <ide.h> 
#include <minix.h>
#include <bcache.h>
#include <balloc.h> 
#include <string.h> 
#include <printk.h> 
#include <dbg.h> 

/* read the super_block */
void read_sb(int dev, struct super_block *sb){
    struct buf *bp;
    bp = bread(dev, 1);

    memcpy(sb, bp->data, sizeof(*sb));
    brelse(bp);
}

void print_sb(){
    struct super_block sb;
    read_sb(0, &sb);

    printl("print_sb:\n");
    printl("    ninodes: %d\n", sb.ninodes);
    printl("    nzones: %d\n", sb.nzones);
    printl("    imap_blk: %d\n", sb.imap_blk);
    printl("    zmap_blk: %d\n", sb.zmap_blk);
    printl("    fst_data_zone: %d\n", sb.fst_data_zone);
    printl("    log_zone_size: %d\n", 1024 << sb.log_zone_size);
    printl("    max_size: %d\n", sb.max_size);
    printl("    magic: 0x%x\n", sb.magic);
    printl("    state: %d\n", sb.state);
}

/* clear a block */
static void bzero(int dev, uint32_t blkno){
    struct buf *bp;

    bp = bread(dev, blkno);
    memset(bp->data, 0, BSIZE);
    bwrite(bp);
    brelse(bp);
}

/* alloc a zeroed disk block*/
uint32_t balloc(int dev){
    uint32_t b, bi, m;
    struct buf *bp;
    struct super_block sb;
    
    bp = 0;
    read_sb(dev,&sb);
    for (b = 0; b < sb.nzones; b += BPB){
        bp = bread(dev, BBLOCK(b, sb.ninodes));
        for(bi = 0; bi < BPB && b + bi < sb.nzones; bi++){
            m = 1 << (bi%8);
            if ((bp->data[bi/8] & m) == 0){ // This block is free
                bp->data[bi/8] |= m;
                bwrite(bp);                 // mark this block as uesed in bitmap
                brelse(bp);
                bzero(dev, b + bi);         // clear block with zero
                return b + bi;
            }
        }
        brelse(bp);
    }
    panic("balloc: out of blocks");
    return ERROR;
}

void bfree(int dev, uint32_t blkno){
    struct buf *bp;
    struct super_block sb;
    uint32_t bi, m;

    read_sb(dev, &sb);

    m = 1 << (blkno%8);
    bi = blkno/8;
    bp = bread(dev, BBLOCK(blkno, sb.ninodes));  // 找到该block对应的bitmap所在的block
    assert(bp->data[bi/8] & m,"bfree: freeing a free block")
    bp->data[bi/8] &= ~m;
    bwrite(bp);
    brelse(bp);
}

