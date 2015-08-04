// std
#include <type.h>
#include <dbg.h>
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
#include <nami.h>
#include <stat.h>

void fs_test(){
    printl("=== fs_test start ===\n");

    /* iread() iwrite() 
    struct inode *ip;
    struct inode *dip;
    char content[100];
    int len;

    ip = nami("/README");
    ilock(ip);
    len = iread(ip, content, 0, ip->size);
    iunlockput(ip);
    printl("fs_test: read %d byte\n", len);
    printl("fs_test: content: [%s]\n", content);

    char write_test[] = "iwrite test :|\n";
    ip = nami("/bin/README");
    ilock(ip);
    len = iwrite(ip, write_test, 0, sizeof(write_test));
    printl("fs_test: write %d byte\n", len);
    len = iread(ip, content, 0, ip->size);
    iunlockput(ip);

    printl("fs_test: content: [%s]\n", content);
    */

    /* dir_link() dir_lookup() 
    ip = nami("/bin/README");
    ilock(ip);
    ip->nlinks++;

    dip = nami("/");
    ilock(dip);
    dir_link(dip, "README2",ip->ino);
    iunlockput(dip);

    iupdate(ip);
    iunlockput(ip);
    */

    /** nami()
    ip = nami("/bin/README1");
    if (!ip){
        printk("NO FOUND\n");
    } **/

    /* stat 
    struct inode *ip;
    ip = nami("/bin/README");
    ilock(ip);
    if (S_ISDIR(ip->mode)) {
        printk("/bin/README is no dir\n");
    }
    iunlockput(ip);

    struct inode *dip;
    dip = nami("/bin");
    ilock(dip);
    if (S_ISDIR(dip->mode)) {
        printk("bin DIR\n");
    }
    iunlockput(dip);
    */

    /*
    // ========== new file 
    struct inode *ip;
    char content[] = "Here is a new file\n";

    ip = ialloc(ROOT_DEV);
    ilock(ip);
    ip->mode |= S_IFREG;

    iwrite(ip, content, ip->size, sizeof(content));

    struct inode *dip = nami("/");
    ilock(dip);
    dir_link(dip, "TEST", ip);

    iunlockput(dip);
    iunlockput(ip);
    */
    
    /* 
    // ========= new dir
    struct inode *ip;
    struct inode *dip;

    ip = ialloc(ROOT_DEV);
    ilock(ip);
    ip->mode |= S_IFDIR;
    iupdate(ip);

    dip = nami("/");
    ilock(dip);

    dir_link(ip, ".", ip);
    dir_link(ip, "..", dip);
    dir_link(dip, "dir", ip);

    iunlockput(dip);
    iunlockput(ip);
    */

    printl("=== fs_test end ===\n");
}

