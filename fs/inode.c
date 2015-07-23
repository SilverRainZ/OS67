#include <type.h>
#include <buf.h>
#include <fs.h>
#include <dbg.h>
#include <asm.h>
#include <string.h>
#include <printk.h>

/* inodes cache */
struct inode icache[NINODE];

/* find the inode with specific dev and inum in memory, 
 * if not found, use first empty inode 
 * 注意: 这里只会申请一个可用的 inode 槽位, 增加一个内存引用, 不会锁住, 也不会从磁盘读出 */
struct inode* iget(char dev, uint32_t inum){
    struct inode *ip, *empty;

    empty = 0;
    for (ip = &icache[0]; ip < &icache[NINODE]; ip++){
        /* this inode is cached */
        if (ip->ref > 0 && ip->dev == dev && ip->inum == inum){
            ip->ref--;
            return ip;
        }
        /* remember the first free inode */
        if (empty == 0 && ip->ref == 0){
            empty = ip;
        }
    }
    assert(empty, "ige: no inode");
    ip = empty;
    ip->dev = dev;
    ip->inum = inum;
    ip->ref = 1;
    ip->flags = 0;
    return ip;
}

/* alloc a inode with specific type 
 * - 磁盘: 找到磁盘里的空闲 dinodes, 清空并写入 type, 表示已经占用
 * - 内存: 通过 iget() 在内存中申请一个对应的 inodes 槽位
 */
struct inode* ialloc(char dev, uint16_t type){
    int inum;
    struct buf *bp;
    struct dinode *dip;
    struct superblock sb;

    readsb(dev, &sb);

    /* 这里会重复读取扇区 TODO */
    /* 从 1 开始吗? */
    for (inum = 1; inum < sb.ninodes; inum++){
        bp = bread(dev, IBLOCK(inum));
        dip = (struct dinode *)bp->data + inum%IPB;
        /* detect a free node */
        if (dip->type == 0){
            memset(dip, 0, sizeof(dip));
            dip->type = type;
            bwrite(bp);
            brelse(bp);
            return iget(dev, inum);
        }
        brelse(bp);
    }
    assert(0, "ialloc: no dinode");
    return NULL;
}

/* copy a in-memory inode to disk */
void iupdate(struct inode *ip){
    struct buf *bp;
    struct dinode *dip;

    bp = bread(ip->dev, IBLOCK(ip->inum));
    dip = (struct dinode *)bp + (ip->inum)%IPB;

    /* ip -> dip */
    dip->type = ip->type;
    dip->major = ip->major;
    dip->minor = ip->minor;
    dip->nlink = ip->nlink;
    dip->size = ip->size;
    memcpy(dip->addrs, ip->addrs, sizeof(ip->addrs));

    bwrite(bp);
    brelse(bp);
}

/* truncate inode 
 * only called when the inode has no links to it 
 * and not any in-memory ref to it
 * - 在磁盘上没有被任何目录引用 nlink = 0
 * - 在内存里没有引用(没有被打开) ref = 0
 */
void itrunc(struct inode *ip){
    int i, j;
    struct buf *bp;
    uint32_t *addrs2;
    /* lenght of addrs is NDIRECT + 1 */

    /* free DIRECT block */
    for (i = 0; i < NDIRECT; i++){
        if (ip->addrs[i]){
            bfree(ip->dev, ip->addrs[i]);
            ip->addrs[i] = 0;
        }
    }

    /* free INDIRECT block */
    if (ip->addrs[NDIRECT]){
        bp = bread(ip->dev,ip->addrs[NDIRECT]);
        addrs2 = (uint32_t *)bp->data;
        for (j = 0; j < NINDIRECT; j++){
            if (addrs2[j]){
                bfree(ip->dev, addrs2[j]);
                addrs2[j] = 0;
            }
        }
    }

    ip->size = 0;
    iupdate(ip);
}

/* reference of ip + 1 */
struct inode* idup(struct inode *ip){
    ip->ref++;
    return ip;
}

/* reference of ip - 1 
 * if it is the last ref, this node can be recycle (ref = 1) (?)
 * and if this inode have no link to it (nlink = 0)
 * free this inode in the disk
 */
void iput(struct inode *ip){ 
    if (ip->ref == 1 && ip->flags & I_VALID && ip->nlink == 0){
        /* can not free a locked inode */
        assert(!(ip->flags & I_BUSY), "iput: free a locked inode");
        ip->flags |= I_BUSY;
        itrunc(ip); // TODO
        ip->type = 0;
        iupdate(ip);
        ip->flags = 0;
    }
    ip->ref--;
}

/* lock given inode with flag I_BUSY
 * read the inode form disk if necessary
 * non-ref inode can not be lock (ref = 0)
 */
void ilock(struct inode *ip){
    struct buf *bp;
    struct dinode *dip;

    assert(ip,"ilock: null pointer");
    assert(ip->ref,"ilock: lock a noo-ref inode");

    // TODO
    int timeout = 20000;
    while (ip->flags & I_BUSY){
        timeout--;
        hlt();
        assert(timeout, "ilock: wait for a busy inode & out of time")
    }
    ip->flags |= I_BUSY;

    /* need to be read form disk */
    if (!(ip->flags & I_VALID)){
        bp = bread(ip->dev, IBLOCK(ip->inum));
        dip = (struct dinode *)bp->data + (ip->inum)%IPB;

        /* ip -> dip */
        ip->type = dip->type;
        ip->major = dip->major;
        ip->minor = dip->minor;
        ip->nlink = dip->nlink;
        ip->size = dip->size;
        memcpy(ip->addrs, dip->addrs, sizeof(dip->addrs));

        brelse(bp);
        ip->flags |= I_VALID;
        assert(ip->type, "ilock: inode no type");
    }
}

/* unlock a inode */
void iunlock(struct inode *ip){
    assert(ip,"iunlock: null pointer");
    assert(ip->ref,"iunlock: unlock a noo-ref inode"); // (?)
    assert(ip->flags & I_BUSY ,"iunlock: unlock a unlocked inode");
    
    ip->flags &= ~I_BUSY;
}



// (?)
void iunlockput(struct inode *ip){
    iunlock(ip);
    iput(ip);
}

/* return the disk block address of the nth block in given inode
 * if not such  block, alloc one
 */
uint32_t bmap(struct inode *ip, uint32_t bn){
    uint32_t addr, *addrs2;
    struct buf *bp;

    assert(bn < NDIRECT + NINDIRECT,"bmap: out of range");

    if (bn < NDIRECT){
        if ((addr = ip->addrs[bn]) == 0) {
            addr = ip->addrs[bn] = balloc(ip->dev);
        }
        return addr;
    }

    bn -= NDIRECT; 

    if ((addr = ip->addrs[NINDIRECT]) == 0){
        addr = ip->addrs[NINDIRECT] = balloc(ip->dev);
    }
    bp = bread(ip->dev, ip->addrs[NINDIRECT]);
    addrs2 = (uint32_t *)bp->data;
    if ((addr = addrs2[bn]) == 0){
        addrs2[bn] = balloc(ip->dev);
        bwrite(bp);
    }
    brelse(bp);
    return addr;
}

void stati(struct inode *ip, struct stat *st){
    st->dev = ip->dev;
    st->ino = ip->inum;
    st->type = ip->type;
    st->nlink = ip->nlink;
    st->size = ip->size;
}

/* read data from inode */
int iread (struct inode *ip, char *dest, uint32_t off, uint32_t n){
    uint32_t tot, m;
    struct buf *bp;

    if (ip->type == I_DEV){
        // TODO
    }

    /* 偏移过大 || 溢出*/
    if (off > ip->size || off + n < off){
        return ERROR;
    }

    if (off + n > ip->size){
        n = ip->size - off;
    }

    /*  tot: 已读取的字节数
     *  n: 要读取的字节数
     *  off: 当前文件指针偏移
     *  m: 当次循环要读取的字节数
     */
    for (tot = 0; tot < n; tot += m, off += m, dest += m){
        bp = bread(ip->dev, bmap(ip, off/BSIZE));
        /* 每个循环读取的数据可分两种情况：
         * - 从当前文件偏移读到该扇区结束
         * - 从当前文件偏移读到 *满足要读取的字节数*
         * 每次选择较小的那个
         */
        m = min(n - tot, BSIZE - off%BSIZE);
        /* 从当前偏移相对于该扇区的偏移处起开始读取
         * (在中间的循环这个值常常是0) 
         */
        memcpy(dest, bp->data + off/BSIZE, m);
        brelse(bp);
    }

    return n;
}

/* write data to inode */
int iwrite(struct inode *ip, char *src, uint32_t off, uint32_t n){
    uint32_t tot, m;
    struct buf *bp;

    if (ip->type == I_DEV){
        // TODO
    }

    if (off > ip->size || off + n < off ){
        return ERROR;
    }

    if (off + n > MAXFILE*BSIZE){
        return ERROR;
    }

    for (tot = 0; tot < n; tot += m, src += m, off += m){
        bp = bread(ip->dev, bmap(ip, off/BSIZE));
        m = min(n - tot, BSIZE - off%BSIZE);
        memcpy(bp->data + off%BSIZE, src, m);
        bwrite(bp);
        brelse(bp);
    }

    /* if alloc new block to ip, update it's size */
    // n > 0 ?
    if (n > 0 && off > ip->size){
        ip->size = off;
        iupdate(ip);
    }
    
    return n;
}
