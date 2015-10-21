// #define __LOG_ON 1
// std
#include <type.h>
#include <dbg.h>
// libs
#include <printk.h>
#include <string.h>
// fs
#include <minix.h> 
#include <inode.h>
#include <dir.h>
#include <p2i.h>
#include <stat.h>
// proc
#include <proc.h>

/* 抽取路径中的第一个名字, 返回余下部分 
 * skip the first elem of path, return this elem
 * - skipelem("a/b/c", name) = "b/c" name = a
 * - skipelem("////a//c", name) = "c" name = a
 * - skipelem("///", name) = 0 
 */
char *skipelem(char *path, char *name){
    char *ns;
    int nlen;

    while (*path == '/'){
        path++;
    }
    if (*path == '\0'){
        return 0;
    }

    ns = path;
    while (*path != '/' && *path != '\0'){
        path++;
    }

    nlen = path - ns;
    if (nlen >= NAME_LEN){
        strncpy(name, ns, NAME_LEN);
    } else {
        strncpy(name, ns, nlen);
        name[nlen] = 0;
    }
    while (*path == '/'){
        path++;
    }
    // printl("skipelem: name: %s, path: %s\n", name, path);
    return path;
}

static struct inode *_path2inode(char *path, int parent, char *name){
    struct inode *ip, *next;

    ip = 0;
    if (*path == '/'){
        ip = iget(ROOT_DEV, ROOT_INO);
    } else {
        ip = idup(proc->cwd);
    }     

    /* NB: 这里是检测 path 的地址而非 值 */
    while ((path = skipelem(path, name)) != 0){
        /* read from disk */
        ilock(ip);
        //print_i(ip);
        if (!S_ISDIR(ip->mode)){
            iunlockput(ip);
            return 0;
        }

        if (parent && *path == '\0'){
            iunlock(ip);
            return ip;
        }

        if ((next = dir_lookup(ip, name, 0)) == 0){
            iunlockput(ip);
            return 0;
        }

        iunlockput(ip);
        ip = next;
    }

    if (parent){
        iput(ip);
        return 0;
    }

    return ip;
}


struct inode *p2i(char *path){
    char name[NAME_LEN];
    return _path2inode(path, 0, name);
}

struct inode *p2ip(char *path, char *name){
    return _path2inode(path, 1, name);
}
