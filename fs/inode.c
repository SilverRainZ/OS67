#include <type.h>
#include <buf.h>
#include <fs.h>
#include <string.h>
#include <printk.h>

/* inodes cache */
struct inode icache[NINODE];

/* find the inode with specific dev and inum in memory, 
 * if not found, use first empty inode 
 * 注意: 这里只会申请一个可用的 inode 槽位, 增加一个内存引用, 不会锁住, 也不会从磁盘读出 */
static struct *inode iget(char dev, uint32_t inum){
    struct buf *bp;
    struct inode *ip, *empty;

    empty = 0;
    for (ip = &icache[0], ip < &icache[NINODE]; ip++){
    /* this inode is cached */
        if (ip->ref > 0 && ip->dev == dev && ip->inum == inum){
            ip->ref--;
            return ip;
        }
    /* remember the first free inode */
        if (empty == 0 && ip->ref == 0){
            empty = ip;
        }

    assert(empty, "ige: no inode");
    ip = empty;
    ip->dev = dev;
    ip->inum = inum;
    ip->ref = 1;
    ip->flag = 0;
    return ip;
}

/* alloc a inode with specific type 
 * - 磁盘: 找到磁盘里的空闲 dinodes, 清空并写入 type, 表示已经占用
 * - 内存: 通过 iget() 在内存中申请一个对应的 inodes 槽位
 */
static struct *inode ialloc(char dev, uint16_t type){
    int inum;
    struct buf *bp;
    struct dinode *dip;
    struct superblock sb;

    readsb(dev, &sb);

    /* 这里会重复读取扇区 TODO */
    /* 从 1 开始吗? */
    for (inum = 1; inum < sb.inodes; inum++){
        bp = bread(ip->dev, IBLOCK(inum));
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
}

/* copy a in-memory inode to disk */
void iupdate(struct *inode ip){
    struct buf *bp;
    struct dinode *dip;

    bp = bread(ip->dev, IBLOCK(ip->inum));
    dip = (struct dinode *)bp + (ip->inum)%IPB;

    /* ip -> dip */
    dip->dev = ip->dev;
    dip->major = ip->major;
    dip->minor = ip->minor;
    dip->nlink = ip->nlink;
    dip->size = ip->size;
    memmove(dip->addrs, ip->addrs, sizeof(ip->addrs));

    bwrite(bp);
    brelse(bp);
}

/* truncate inode 
 * only called when the inode has no links to it 
 * and not any in-memory ref to it
 * - 在磁盘上没有被任何目录引用 nlink = 0
 * - 在内存里没有引用(没有被打开) ref = 0
 */
static void itrunc(struct inode *ip){
    int i, j;
    struct buf *bp;
    uint32_t *addrs2;

    // TODO
}

/* reference of ip + 1 */
struct inode* idup(struct inodes *ip){
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
        iupdat(ip);
        ip->flag = 0;
    }
    ip->ref--;
}

/* lock given inode with flag I_BUSY
 * read the inode form disk if necessary
 * non-ref inode can not be lock (ref = 0)
 */
void ilock(struct indoe *ip){
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
        ip->dev = dip->dev;
        ip->major = dip->major;
        ip->minor = dip->minor;
        ip->nlink = dip->nlink;
        ip->size = dip->size;
        memmove(ip->addrs, dip->addrs, sizeof(dip->addrs));

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

void bmap(){
    // TODO
}

void stati(struct inode *ip, struct stat *st){
    st->dev = ip->dev;
    st->ino = ip->inum;
    st->type = ip->type;
    st->nlink = ip->nlink;
    st->size = ip->size;
}
