#include <type.h>
#include <buf.h>
#include <fs.h>
#include <printk.h>

/* inodes cache */
struct inode icache[NINODE];

static struct *inode iget(char dev, uint32_t inum){

}

static struct *inode ialloc(char dev, uint16_t type){

}

void iupdate(struct *inode ip){

}

struct inode* idup(struct inodes *ip){

}

void ilock(struct indoe *ip){

}

void iunlock(struct inode *ip){

}

void iput(struct inode *ip){

}

void iunlockput(struct inode *ip){

}
