#define __LOG_ON 1
// std
#include <type.h>
// libs
#include <printk.h>
#include <string.h>
// fs
#include <inode.h>
#include <p2i.h>
// mm
#include <vmm.h>
// proc
#include <proc.h>
#include <exec.h>
#include <elf.h>

static void print_elfhdr(struct elf32hdr *eh){
    printl("print_eh:\n");
    printl("    magic: 0x%x\n", eh->magic);
    printl("    name: %s\n", eh->elf);
    printl("    entry: 0x%x\n", eh->entry);
    printl("    phoff: 0x%x\n", eh->phoff);
    printl("    phnum: %d\n", eh->phnum);
}

static void print_proghdr(struct proghdr *ph){
    printl("print_proghdr:\n");
    printl("    type: 0x%x\n", ph->type);
    printl("    off: 0x%x\n", ph->off);
    printl("    vaddr: 0x%x\n", ph->vaddr);
    printl("    paddr: 0x%x\n", ph->paddr);
    printl("    filesz: 0x%x\n", ph->filesz);
    printl("    memsz: 0x%x\n", ph->memsz);
    printl("    flag: 0x%x\n", ph->flag);
    printl("    align: 0x%x\n", ph->align);
}

int exec(char *path){
    int i;
    char *s, *name;
    uint32_t sz, sp, off;
    pde_t *pgdir, *old_pgdir;
    struct inode *ip;
    struct elf32hdr eh;
    struct proghdr ph;
    
    printl("exec: try to read `%s` form disk...\n", path);

    if ((ip = p2i(path)) == 0){
        return -1;
    }
    ilock(ip);

    pgdir = 0;
    i = off = 0;

    // read elf header
    if (iread(ip, (char *)&eh, 0, sizeof(eh)) < (int)sizeof(eh)){
        goto bad;
    }

    printl("exec: parsering elf\n");

    print_elfhdr(&eh);

    if (eh.magic != ELF_MAGIC){
        goto bad;
    }

    printl("exec: load program section to memory\n");

    // load program to memory
    sz = 0;
    for (i = 0, off = eh.phoff; i < eh.phnum; i++, off += sizeof(ph)){
        if (iread(ip, (char *)&ph, off, sizeof(ph)) != sizeof(ph)){
            goto bad;
        }
        print_proghdr(&ph);
        printl("exec: 1\n");
        if (ph.type != ELF_PROG_LOAD){
            continue;
        }
        printl("exec: 2\n");
        if (ph.memsz < ph.filesz){
            goto bad;
        }
        printl("exec: 3\n");
        if ((sz = uvm_alloc(pgdir, sz, ph.vaddr + ph.memsz)) == 0){
            goto bad;
        }
        printl("exec: 4\n");
        if (uvm_load(pgdir, ph.vaddr, ip, ph.off, ph.filesz) < 0){
            goto bad;
        }
        iunlockput(ip);
        ip = 0;
    }

    printl("exec: build user stack(TODO)\n");
    /* build user stack */
    sz = PAGE_ALIGN_UP(sz);
    if ((sz = uvm_alloc(pgdir, sz, sz + 2*PAGE_SIZE)) == 0){
        goto bad;
    }
    // leave a page unaccessable TODO
    sp = sz;

    /*
    for (argc = 0; argv[argc]; argc++){
        if (argc > MAX_ARGC) {
            goto bad;
        }
        // "+1" leava room for '\0'  "&~3" align 4
        sp = sp - (strlen(argv[argc]) + 1) & ~3;    
    }
    */

    printl("exec: prepare for new process\n");

    for (name = s = path; *s; s++){
        if (*s == '/'){
            name = s + 1;
        }
    }

    strncpy(proc->name, name, sizeof(proc->name));

    old_pgdir = proc->pgdir;
    proc->pgdir = pgdir;
    proc->size = sz;
    proc->fm->eip = eh.entry;
    proc->fm->user_esp = sp;
    uvm_switch(proc);
    uvm_free(old_pgdir);
    return 0;

bad:
    printl("exec: bad");
    if (pgdir){
        uvm_free(pgdir);
    }
    if (ip){
        iunlockput(ip);
    }
    return -1;
}

