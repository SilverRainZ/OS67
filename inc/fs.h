#ifndef __FS_H
#define __FS_H
#include <type.h>
#include <buf.h>

/*  Struct of file system
 *      0           1           2           ...         ...
 *  ===========================================================
 *  bootsector | superblock | inodes ...| bit map ...| data ...
 *  ===========================================================
 */

/* superblock is located in the second sector of disk */
struct superblock{
    uint32_t size;      // size of disk (how many blocks(not only data block) it have)
    uint32_t nblocks;   // number of data blocks
    uint32_t ninodes;   // number of inodes
    // uint32_t nlog;   // i don't want to use log now
};

#define NDIRECT 12  // (?)

/* On-disk inodes structure */
struct dinode{
    int16_t type;
    uint16_t major;     // (?)
    uint16_t minor;     // (?)
    uint32_t nlink;     // number of links to this node
    uint32_t size;      // size of file (byte)
    uint32_t addrs[NDIRECT + 1];
};

/* in-memory copy of an inode */
struct inode{
    uint32_t dev;
    uint32_t inum;
    int32_t ref;
    int32_t flags;

    /* copy of a on-disk inodes */
    int16_t type;
    uint16_t major;     // (?)
    uint16_t minor;     // (?)
    uint32_t nlink;     // number of links to this node
    uint32_t size;      // size of file (byte)
    uint32_t addrs[NDIRECT + 1];
};
/* inode per block */
#define IPB (BSIZE/sizeof(struct inode))

/* block contain inode i */
#define IBLOCK(i) ((i)/IPB + 2)     

/* bit number a  block contain (1 byte has 8 bits)*/
#define BPB (BSZIE*8)

/* block contain bit b */
/* 取得包含第b bit的block号, ninodes 只可能是 superblock.ninodes  */
#define BBLOCK(b, ninodes) (b/BPB + (ninodes)/IPB + 3)

/******functions in file fs/bcache.c buffer cache *****/
void bcache_init();
struct buf* bread(char dev, uint32_t lba);
void bwrite(struct buf* b);
void brelse(struct buf *b);

/******functions in file fs/balloc.c  block alloc *****/
void readsb(int dev, struct superblock *sb);
uint32_t balloc(int dev);
void bfree(int dev, uint32_t blkn);

#endif
