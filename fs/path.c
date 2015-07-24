#include <type.h>
#include <dbg.h>
#include <printk.h>
#include <string.h>
#include <fs.h> 

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
    if (*path == 0){
        return 0;
    }

    ns = path;
    while (*path != '/' && *path != '0'){
        path++;
    }

    nlen = path - ns;
    if (nlen >= DIRSIZE){
        strncpy(name, ns, DIRSIZE);
    } else {
        strncpy(name, ns, nlen);
        name[nlen] = 0;
    }
    while (*path == '/'){
        path++;
    }
    return path;
}

struct inode *namex(char *path, int parent, char *name){
    struct inode *ip, *next;

    if (*path == '/'){
        ip = iget(ROOT_DEV, ROOT_INO);
    } else {
        // TODO
        panic("namex: root dir");
    }

    while ((path = skipelem(path, name)) != 0){
        /* read from disk */
        ilock(ip);
        if (ip->type != I_DIR){
            iunlockput(ip);
            return 0;
        }

        if (parent && *path != '\0'){
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


struct inode *namei(char *path){
    char name[DIRSIZE];
    return namex(path, 0, name);
}

struct inode *nameiparent(char *path, char *name){
    return namex(path, 1, name);
}
