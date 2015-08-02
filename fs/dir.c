// std
#include <type.h>
#include <dbg.h>
// libs
#include <string.h>
#include <printk.h>
// fs
#include <minix.h>
#include <inode.h>


/* lookup a entry by name in specific dir inode */
struct inode* dir_lookup(struct inode *dip, char *name, uint32_t *poff){
    uint32_t off;
    struct dir_entry de;

    printl("dir_lookup: inode-%d, lookup: %s\n", dip->ino, name);

    //assert(dip->mode == T_DIR, "dir_lookup: no dir");

    for (off = 0; off < dip->size; off += sizeof(de)){
        if (iread(dip, (char*)&de, off,sizeof(de)) != sizeof(de)){
            panic("dir_lookup: fault when read");
        }

        if (de.ino == 0){
            continue;
        }

        printl("inode-%d named: [%s]\n", de.ino, de.name);

        if (strncmp(name, de.name, NAME_LEN) == 0){
            printl("dir_lookup: found\n");

            /* found */
            if (poff){
                *poff = off;
            }
            return iget(dip->dev, de.ino);
        }
    }

    printl("no found\n");
    /* no found */
    return NULL;
}

/* write a new dir entry(name &.ino) into the dir inode */
int dir_link(struct inode *dip, char *name, uint32_t ino){
    uint32_t off;
    struct dir_entry de;
    struct inode *ip;

    printl("dir_link: link inode-%d named %s to dir inode-%d\n", ino, name, dip->ino);

    /* is this name existed? */
    if ((ip = dir_lookup(dip, name, 0)) != 0){
        iput(ip); // dir_lookup return a inode by call iget() 
        return ERROR;
    }

    for (off = 0; off < dip->size; off += sizeof(de)){
        if (iread(dip, (char *)&de, off, sizeof(de)) != sizeof(de)){
            panic("dir_link: fault when read");
        }

        /* found a free entry */
        if (de.ino == 0){
            break;
        }
    }
    strncpy(de.name, name, NAME_LEN);
    de.ino = ino;

    if (iwrite(dip, (char *)&de, off, sizeof(de)) != sizeof(de)){
        panic("dir_link: fault when write");
    }
    return OK;
}


