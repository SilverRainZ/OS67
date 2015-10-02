// std
#include <type.h>
#include <dbg.h>
// libs
#include <printk.h>
#include <string.h>
// fs
#include <minix.h>
#include <dir.h>
#include <p2i.h>
#include <stat.h>
#include <file.h>

static struct file ftable[NFILE];

void file_init(){
    memset(ftable, 0, sizeof(struct file)*NFILE);
}

struct file *falloc(){
    int i;

    for (i = 0; i < NFILE; i++){
        if (ftable[i].ref == 0){
            ftable[i].ref = 1;
            return &ftable[i];
        }
    }
    return 0;
}

struct file *fdup(struct file *fp){
    assert(fp->ref >= 1, "fdup: file's ref < 1");

    fp->ref++;
    return fp;
}

void fclose(struct file *fp){
    assert(fp->ref >= 1, "fclose: file's ref < 1");

    fp->ref--;
    if (fp->ref > 0){
        return;
    }

    if (fp->type == F_PIPE){
        // TODO
        panic("fread: no support");
        return;
    }
    if (fp->type == F_INODE){
        iput(fp->ip);
        fp->type = F_NONE;

        return;
    }
}

int fstat(struct file *fp, struct stat *st){

    if (fp->type == F_INODE){
        ilock(fp->ip);
        istat(fp->ip, st);
        iunlock(fp->ip);

        return 0;
    }
    return -1;
}

int fread(struct file *fp, char *addr, uint32_t n){
    int len = 0;

    if (!fp->readable){
        return ERROR;
    }

    if (fp->type == F_PIPE){
        // TODO
        panic("fread: no support");
    }

    if (fp->type == F_INODE){
        ilock(fp->ip);
        if ((len = iread(fp->ip, addr, fp->off, n)) > 0){
            fp->off += len;
        }
        iunlock(fp->ip);
    }

    return len;
}

int fwrite(struct file *fp, char *addr, uint32_t n){

    if (!fp->writeable){
        return ERROR;
    }

    if (fp->type == F_PIPE){
        // TODO
        panic("fwrite: no support");
    }

    if (fp->type == F_INODE){
        ilock(fp->ip);
        if (iwrite(fp->ip, addr, fp->off, n) != (int)n){
            panic("fwrite: fault");
        }
        iunlock(fp->ip);
        return n;
    }

    panic("fwrite: wrong type");
    return -1;
}


