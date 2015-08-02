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

void fs_test(){
    // char content[100];
    struct inode *ip;
    struct inode *dip;
    // int len;

    printl("=== fs_test start ===\n");

    /*
    ip = nami("/README");
    ilock(ip);
    print_i(ip);
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


    dip = nami("/");
    ip = nami("/bin/README");
    ilock(dip);
    dir_link(dip, "README2",ip->ino);
    iunlockput(dip);

    printl("=== fs_test end ===\n");
}

