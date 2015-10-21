// #define __LOG_ON 1

/* sb.c
 * read or dump superblock
 * superblock can not be modified
 */
// std
#include <type.h>
// libs
#include <string.h>
#include <printk.h>
// fs
#include <buf.h>
#include <minix.h>
#include <bcache.h>


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


