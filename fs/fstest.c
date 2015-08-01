#include <printk.h>
#include <ide.h>
#include <bcache.h>
#include <minix.h>
#include <sb.h>
#include <bitmap.h>
#include <inode.h>
#include <dir.h>
#include <nami.h>

void fs_test(){
    printl("=== fs_test start ===\n");
    char content[100];

    struct inode *ip;

    ip = nami("/README");
    printl("=== nami end ===\n");
    
    ilock(ip);
    print_i(ip);
    printl("=== print_i end ===\n");
    int len = iread(ip, content, 0, ip->size);

    printl("read %d byte\n", len);
    printl("Content: %s\n", content);
    
    printl("=== fs_test end ===\n");
}

