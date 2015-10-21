// #define __LOG_ON 1
// std
#include <type.h>
#include <dbg.h>
#include <asm.h>
// libs
#include <printk.h>
#include <string.h>
// drv
#include <ide.h>
// fs
#include <minix.h>
#include <sb.h>
#include <bcache.h>
#include <bitmap.h>
#include <inode.h>
#include <dir.h>
#include <p2i.h>
#include <stat.h>

void fs_test(){
    struct inode *ip, *dip;
    printl("=== fs_test start ===\n");

    printl("================ add a new directory `/bin`\n");
    char dirname[] = "bin";

    ip = ialloc(ROOT_DEV);
    ilock(ip);
    ip->mode |= S_IFDIR;
    ip->nlinks = 2; // "." and "/"
    iupdate(ip);

    dip = p2i("/");
    ilock(dip);

    dip->nlinks++;

    dir_link(ip, ".", ip);
    dir_link(ip, "..", dip);
    dir_link(dip, dirname, ip);

    print_i(ip);
    print_i(dip);

    iunlockput(ip);
    iunlockput(dip);

    printl("================ add a new file `/bin/fstest`\n");
    char fname[] = "fstest";
    char content[] = "fs_test\n";
    ip = ialloc(ROOT_DEV);
    ilock(ip);
    ip->mode |= S_IFREG;
    ip->nlinks = 1;
    iwrite(ip, content, ip->size, sizeof(content));

    dip = p2i("/bin");
    ilock(dip);
    dir_link(dip, fname, ip);

    print_i(ip);
    print_i(dip);
    iunlockput(ip);
    iunlockput(dip);

    printl("================ delete file `/bin/fstest`\n");
    char dname[NAME_LEN];
    struct dir_entry de;
    uint32_t off;

    dip = p2ip("/bin/fstest", dname);
    ilock(dip);
    ip = dir_lookup(dip, dname, &off);
    ilock(ip);

    memset(&de, 0, sizeof(de));
    iwrite(dip, (char *)&de, off, sizeof(de));
    iupdate(dip);
    ip->nlinks--;
    iupdate(ip);

    print_i(ip);
    print_i(dip);
    iunlockput(ip);
    iunlockput(dip);
    
    printl("=== fs_test end ===\n");
}

