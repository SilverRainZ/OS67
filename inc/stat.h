#ifndef __STAT_H
#define __STAT_H

#include <type.h>

/* - - - - - - - | - - - - - - - - - 
 * r d   p         r w x r w x r w x
 */
/*ref: http://minix1.woodhull.com/manpages/man2/stat.2.html */
/* http://faculty.qu.edu.qa/rriley/cmpt507/minix/stat_8h-source.html#l00016 */

/* octal */
#define S_IFMT  0170000    /* type of file */
#define S_IFIFO 0010000    /* named pipe */
#define S_IFCHR 0020000    /* character special */
#define S_IFDIR 0040000    /* directory */
#define S_IFBLK 0060000    /* block special */
#define S_IFREG 0100000    /* regular */
#define S_IFLNK 0120000    /* symbolic link (Minix-vmd) */

#define S_RWX 0000777

#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)     /* is a reg file */
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)     /* is a directory */
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)     /* is a char spec */
/* all device are regard as char device */
// #define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)     /* is a block spec */
// #define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)     /* is a symlink */
// #define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)     /* is a pipe/FIFO */

 struct stat {
     uint16_t dev;
     uint16_t ino;
     uint16_t mode;
     uint32_t size;
};

#endif
