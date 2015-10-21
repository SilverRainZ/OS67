// #define __LOG_ON 1
// std
#include <type.h>
#include <dbg.h>
#include <asm.h>
// libs
#include <printk.h>
#include <string.h>
// fs
#include <inode.h>
#include <p2i.h>
// mm
#include <pmm.h>
#include <vmm.h>
// proc
#include <proc.h>
#include <exec.h>
#include <elf.h>

void print_elfhdr(struct elf32hdr *eh){
    printl("print_elfhdr:\n");
    printl("    magic: 0x%x\n", eh->magic);
    printl("    elf: %s\n", eh->elf);
    printl("    entry: 0x%x\n", eh->entry);
    printl("    phoff: 0x%x\n", eh->phoff);
    printl("    phnum: %d\n", eh->phnum);
}

void print_proghdr(struct proghdr *ph){
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

int exec(char *path, char **argv){
    int i;
    char *s, *name;
    uint32_t sz, sp, off, argc, pa, ustack[3 + MAX_ARGC + 1];
    pde_t *pgdir, *old_pgdir;
    struct inode *ip;
    struct elf32hdr eh;
    struct proghdr ph;
    
    printl("exec: try to read `%s` form disk...\n", path);

    pgdir = 0;
    i = off = 0;

    pgdir = (pde_t *)pmm_alloc();
    kvm_init(pgdir);

    // exception handle pgdir
    //
    if ((ip = p2i(path)) == 0){
        goto bad;
    }
    ilock(ip);

    // read elf header
    if (iread(ip, (char *)&eh, 0, sizeof(eh)) < (int)sizeof(eh)){
        goto bad;
    }

    printl("exec: parsering elf\n");
    // print_elfhdr(&eh);

    if (eh.magic != ELF_MAGIC){
        goto bad;
    }

    printl("exec: load program section to memory\n");

    // load program to memory
    sz = USER_BASE;
    for (i = 0, off = eh.phoff; i < eh.phnum; i++, off += sizeof(ph)){
        if (iread(ip, (char *)&ph, off, sizeof(ph)) != sizeof(ph)){
            goto bad;
        }
        // print_proghdr(&ph);
        if (ph.type != ELF_PROG_LOAD){
            continue;
        }
        if (ph.memsz < ph.filesz){
            goto bad;
        }
        if ((sz = uvm_alloc(pgdir, sz, ph.vaddr + ph.memsz)) == 0){
            goto bad;
        }
        if (uvm_load(pgdir, ph.vaddr, ip, ph.off, ph.filesz) < 0){
            goto bad;
        }
    }

    iunlockput(ip);
    ip = 0;

    printl("exec: build user stack\n");
    /* build user stack */
    sz = PAGE_ALIGN_UP(sz);
    if ((sz = uvm_alloc(pgdir, sz, sz + 2*PAGE_SIZE)) == 0){
        goto bad;
    }

    /* leave a unaccessable page between kernel stack */
    if (vmm_get_mapping(pgdir, sz - 2*PAGE_SIZE, &pa) == 0){  // sz is no mapped
        goto bad;
    }
    vmm_map(pgdir, sz - 2*PAGE_SIZE, pa, PTE_K | PTE_P | PTE_W);

    sp = sz;
    if (vmm_get_mapping(pgdir, sz - PAGE_SIZE, &pa) == 0){  // sz is no mapped
        goto bad;
    }
    pa += PAGE_SIZE;

    printl("exec: argv ");
    for (argc = 0; argv[argc]; argc++){
        if (argc > MAX_ARGC) {
            goto bad;
        }
        printl("%d: %s ",argc, argv[argc]);
        // "+1" leava room for '\0'  "&~3" align 4
        sp = (sp - (strlen(argv[argc]) + 1)) & ~3;    // sync with pa
        pa = (pa - (strlen(argv[argc]) + 1)) & ~3;    

        strcpy((char *)pa, argv[argc]);
        ustack[3+argc] = sp;  // argv[argc]
    }
    printl("\n");

    ustack[3+argc] = 0;

    ustack[0] = 0xffffffff;
    ustack[1] = argc;   // count of arguments
    ustack[2] = sp - (argc+1)*4;    // pointer of argv[0]

    sp -= (3 + argc + 1)*4;
    pa -= (3 + argc + 1)*4;
    memcpy((void *)pa, ustack, (3 + argc + 1)*4);   // combine

    for (name = s = path; *s; s++){
        if (*s == '/'){
            name = s + 1;
        }
    }
    printl("exec: prepare for new process `%s`\n", name);

    cli();  // 不清楚此处是否会有死锁或冲突的风险， 反正关中断也不要钱
    strncpy(proc->name, name, sizeof(proc->name));

    old_pgdir = proc->pgdir;
    proc->pgdir = pgdir;
    proc->size = sz - USER_BASE;
    proc->fm->eip = eh.entry;
    proc->fm->user_esp = sp;
    uvm_switch(proc);

    printl("exec: free old pgdir\n");
    uvm_free(old_pgdir);
    old_pgdir  = 0;
    old_pgdir ++;
    sti();

    return 0;

bad:
    printl("exec: bad\n");
    if (pgdir){
        uvm_free(pgdir);
    }
    if (ip){
        iunlockput(ip);
    }
    return -1;
}

