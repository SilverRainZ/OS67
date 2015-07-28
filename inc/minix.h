#ifndef __MINIX_H
#define __MINIX_H
/* Minix v1 file system structure
 *  zone:  0           1              2             2 + imap_blk        ...         ...
 *  +--------------------------------------------------------------------------------------+
 *  | bootsector | superblock | inode bitmap ... | zone bitmap ... | inodes ... | data ... | 
 *  +--------------------------------------------------------------------------------------+
 *  1 zone = 2 block = 1024 byte
 */

#include <type.h> 

#define ROOT_DEV 0
#define ROOT_INO 1

/* size of a virtual block (zone) (byte) */
#define BSIZE 1024
/* size of a block in disk (byte) */
#define PHY_BSIZE 512 

/* minix superblock, locate at block 2 (zone 1)*/
struct super_block{
    uint16_t ninodes;       // number of inodes
    uint16_t nzones;        // number of zones
    uint16_t imap_blk;      // space uesd by inode map (block)
    uint16_t zmap_blk;      // space used by zone map   (block)
    uint16_t fst_data_zone; // first zone with `file` data
    uint16_t log_zone_size; // size of a data zone = 1024 << log_zone_size

    uint32_t max_size;      // max file size (byte)
    uint16_t magic;         // magic number
    uint16_t state;         // (?) mount state
};

/* inode status flag */
#define I_BUSY 0x1
#define I_VALID 0x2

/* inode type flag */
#define T_FREE 0x0
#define T_DIR 0x1
#define T_FILE 0x2
#define T_DEV 0x3

/* in-disk minix inode */
struct dinode{
    uint16_t mode;  // file type and RWX(unused) bit
    uint16_t uid;   // identifies the user who owns the file (unused)
    uint32_t size;  // file size (byte)
    uint16_t mtime; // time since Jan. 1st, 1970 (second) (unused)
    uint8_t gid;    // owner's group (unused)
    uint8_t nlinks; // number of dircetory link to it

    /* zone[0] - zone[6] point to to direct blocks
     * zone[7] points to a indirect block table
     * zone[8]  points to a double indirect block table (unused)
     * so this file system's real max file size = (7 + 512) * 1024 byte = 519 kb
     */
    uint16_t zone[9]; 
};

/* in-memorty inode */
struct inode{
    uint16_t dev;
    uint32_t inum;
    uint16_t ref;
    uint16_t flags;
    uint16_t atime; 
    uint16_t ctime; 
    
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint16_t mtime;
    uint8_t gid;
    uint8_t nlinks;
    uint16_t zone[9];
};

#define DIRSIZE 14
/* minix directroy entry */
struct dir_entry{
    uint16_t inum;
    char name[DIRSIZE];
};

/* inode per block */
#define IPB (BSIZE/sizeof(struct inode))
/* bit number a  block contain (1 byte has 8 bits)*/
#define BPB (BSIZE*8)

/* block contain inode i 
 * NB: inode number starts at 1 */
#define IBLOCK(sb, i) (2 + ((sp)->imap_blk) + ((sp)->zmap_blk) + ((i) - 1)/IPB)
/* block contain bit b */
/* 取得包含第b bit的block号, ninodes 只可能是 super_block.ninodes  */
#define BBLOCK(sb, b) (b/BPB + (sb->ninodes)/IPB + (sp->imp_blk) + (sp->zmap_blk))

#define NDIRECT 7 // 一个 i 节点直接管辖的块数目
#define NINDIRECT (BSIZE/sizeof(uint32_t))  // 1024/4 = 256
#define MAXFILE (NDIRECT + NINDIRECT)       // 


#endif
