#include <type.h>
#include <dbg.h>
#include <string.h>
#include <printk.h>
#include <fs.h>

/* lookup a entry by name in specific dir inode */
struct inode* dir_lookup(struct inode *dirip, char *name, uint32_t *poff){
    uint32_t off;
    struct dirent de;

    assert(dirip->type == I_DIR, "dir_lookup: no dir");
    // assert(poff, "dir_loopup: null pointer poff");

    for (off = 0; off < dirip->size; off += sizeof(de)){
        if (iread(dirip, (char*)&de, off,sizeof(de)) != sizeof(de)){
            panic("dir_lookup: fault when read");
        }

        if (de.inum == 0){
            continue;
        }

        if (strncmp(name, de.name, 14) == 0){
            /* found */
            if (poff){
                *poff = off;
            }
        }
        return iget(dirip->dev, de.inum);
    }

    /* no found */
    return 0;
}

/* write a new dir entry(name & inum) into the dir inode */
int dir_link(struct inode *dirip, char *name, uint32_t inum){
    uint32_t off;
    struct dirent de;
    struct inode *ip;

    /* is this name existed? */
    if ((ip = dir_lookup(dirip, name, 0)) != 0){
        iput(ip); // (?) 
        return ERROR;
    }

    for (off = 0; off < dirip->size; off += sizeof(de)){
        if (iread(dirip, (char *)&de, off, sizeof(de)) != 0){
            panic("dir_link: fault when read");
        }
        /* found a free entry */
        if (de.inum == 0){
            break;
        }
        
    }
    strncpy(de.name, name, DIRSIZE);
    de.inum = inum;

    if (iwrite(dirip, (char *)&de, off, sizeof(de)) != sizeof(de)){
        panic("dir_link: fault when write");
    }
    return 0;
}


