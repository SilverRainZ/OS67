/* ide.c
 * This file is modified form xv6
 * use DMA, assume we hava only a disk
 * 假设系统只有一个ATA磁盘, 并且始终存在
 * 使用LBA28寻址, 使用DMA存取数据
 * 注意磁盘镜像是通过 make fs 生成的rootfs.img文件, 而内核单独存在于floppy.img中
 * 因此你无法通过这里的函数访问到内核.
 */

#include <ide.h> 
#include <buf.h>
#include <asm.h>
#include <type.h>
#include <isr.h>
#include <heap.h>
#include <string.h>
#include <printk.h>
#include <dbg.h>

/* 缓冲区定义在 bcache.c */
static struct buf *idequeue = NULL;    // buffer queue of disk request

/* wait for disk until it ready, checkerror when checkerr=1 */
static int ide_wait(int checkerr){
    int timeout = 20000;
    int r;
    /* 循环检测直到不再IDE_BSY */
    while ((r = inb(IDE_PORT_STATUS)) & IDE_BSY){
        --timeout;
        assert(timeout,"ide_wait: timeout");
    }
    if (checkerr && (r & (IDE_DF|IDE_ERR))){
        return ERROR;
        // assert()
    }
    return OK;
}


static void ide_start(struct buf *b){
    assert(b, "idestart: buffer is null");
    ide_wait(0);
    outb(IDE_PORT_ALTSTATUS, 0);  // generate interrupt
    outb(IDE_PORT_SECT_COUNT, 1);  // number of sectors 
    outb(IDE_PORT_LBA0, b->lba & 0xff);
    outb(IDE_PORT_LBA1, (b->lba >> 8) & 0xff);
    outb(IDE_PORT_LBA2, (b->lba >> 16) & 0xff);
    /* IDE_PORT_CURRENT = 0x101dhhhh d = driver hhhh = head*/
    /* but 0xe0 = 1010000 (? TODO */
    outb(IDE_PORT_CURRENT, 0xe0 | ((b->dev&1)<<4) | ((b->lba>>24)&0x0f)); 
    if(b->flags & B_DIRTY){   // write
        outb(IDE_PORT_CMD, IDE_CMD_WRITE);
        outsl(IDE_PORT_DATA, b->data, 512/4);
    } else {                  // read
        outb(IDE_PORT_CMD, IDE_CMD_READ);
    }
}

void ide_handler(struct regs_s *r){
    struct buf *b;
    /* if queue is empty */
    if ((b = idequeue) == 0){
        return; // nothing to do
    }
    /* remove the head of queue */
    idequeue = b->qnext;
    if (!(b->flags & B_DIRTY) && (ide_wait(1) >= 0)){
        insl(IDE_PORT_DATA, b->data, 512/4);
    }

    /* 缓冲区可用 */
    b->flags |= B_VALID;
    /* 数据是最新的 */
    b->flags &= ~B_DIRTY;

    if (idequeue){
        ide_start(idequeue);
    }
}
void ide_init(){
    /* NB: 一切从简, 不需要检测磁盘的存在 */
    /* IRQ14 = Primary ATA Hard Disk */
    irq_install_handler(14,ide_handler);
    ide_wait(0);
}

void ide_rw(struct buf *b){
    assert(b->flags & B_BUSY,"ide_rw: buf not busy");
    assert((b->flags & (B_VALID|B_DIRTY)) != B_VALID,"ide_rw: nothing to do");

    /* append b to idequeue */
    /* nb: 这里的双重指针要小心 */
    struct buf **pp;
    b->qnext = 0; 
    for (pp = &idequeue; *pp; pp = &(*pp)->qnext);
    *pp = b;
    
    /* if b is the head of queue, read/write it now */
    if (idequeue == b){
        ide_start(b);
    }
    // wait for the request to finish 
    while ((b->flags & (B_VALID|B_DIRTY)) != B_VALID) {
        hlt();  // TODO 
    }
}

void ide_print_blk(struct buf *b){
    printk("flag: ");
    if (b->flags & B_BUSY) printk("B_BUSY ");
    if (b->flags & B_DIRTY) printk("B_DIRTY");
    if (b->flags & B_VALID) printk("B_VALID");
    printk("\n");
    printk("dev: %d, lba: %d\n",b->dev, b->lba);
    int i,j;
    for (i = 0; i < 512; i += 16){
        printk("%x: ",b->lba*512 + i);
        for (j = i; j < i + 16; j++){
            printk("%x ",b->data[j]);
        }
        printk("\n");
    }
}
struct buf buffer;  // used for test
void ide_test(){
    buffer.dev = 0;
    buffer.lba = 2;
    buffer.flags = B_BUSY;
    ide_rw(&buffer);
    ide_print_blk(&buffer);
    int i = 0;
    for (i = 0; i < 512; i++){
        buffer.data[i] = 1;
    }
    buffer.flags = B_BUSY|B_DIRTY;
    ide_rw(&buffer);

    buffer.flags = B_BUSY;
    ide_rw(&buffer);
    ide_print_blk(&buffer);
}
