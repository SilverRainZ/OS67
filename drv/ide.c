/* ide.c
 * This file is modified form xv6
 * use DMA, assume we hava only a disk
 * 假设系统只有一个ATA磁盘, 并且始终存在
 * 使用LBA28寻址, 使用 PIO 存取数据
 * 注意磁盘镜像是通过 make fs 生成的rootfs.img文件, 而内核单独存在于floppy.img中
 * 因此你无法通过这里的函数访问到内核.
 */
// std
#include <type.h>
#include <dbg.h>
#include <asm.h>
// x86
#include <isr.h>
//mm
#include <heap.h>
// libs
#include <string.h>
#include <printk.h>
// drv
#include <ide.h> 
// fs
#include <buf.h>

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

    /* get physical block number */
    uint32_t phy_blkn = b->blkno*(BSIZE/PHY_BSIZE);

    outb(IDE_PORT_ALTSTATUS, 0);  // generate interrupt

    /* number of sectors, read 2 sector for once  */
    outb(IDE_PORT_SECT_COUNT, BSIZE/PHY_BSIZE);  

    outb(IDE_PORT_LBA0, phy_blkn & 0xff);
    outb(IDE_PORT_LBA1, (phy_blkn  >> 8) & 0xff);
    outb(IDE_PORT_LBA2, (phy_blkn >> 16) & 0xff);
    /* IDE_PORT_CURRENT = 0x101dhhhh d = driver hhhh = head*/
    /* but 0xe0 = 1010000 (? TODO */
    outb(IDE_PORT_CURRENT, 0xe0 | ((b->dev & 1) << 4) | ((phy_blkn >> 24) & 0x0f)); 

    if(b->flags & B_DIRTY){   // write
        // printl("ide_start: write blk-%d\n", b->blkno);
        outb(IDE_PORT_CMD, IDE_CMD_WRITE);
        outsl(IDE_PORT_DATA, b->data, BSIZE/4);
    } else {                  // read
        // printl("ide_start: read blk-%d\n", b->blkno);
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
        insl(IDE_PORT_DATA, b->data, BSIZE/4);
    }

    /* 缓冲区可用 */
    b->flags |= B_VALID;
    /* 数据是最新的 */
    b->flags &= ~B_DIRTY;

    //printl("ide_handler: blk-%d VALID \n", b->blkno);
    if (idequeue){
        ide_start(idequeue);
    }
}

void ide_init(){
    /* NB: 一切从简, 不需要检测磁盘的存在 */
    /* IRQ14 = Primary ATA Hard Disk */
    irq_install(IRQ_IDE, ide_handler);
    ide_wait(0);
}

void ide_rw(struct buf *b){
    assert(b->flags & B_BUSY,"ide_rw: buf not busy");
    assert((b->flags & (B_VALID|B_DIRTY)) != B_VALID,"ide_rw: nothing to do");

    /* append b to idequeue */
    /* nb: 这里的双重指针要小心 */
    struct buf **pp;
    b->qnext = 0; 
    //printl("ide_rw: request queue: ");
    for (pp = &idequeue; *pp; pp = &(*pp)->qnext){
        //printl("%d -> ",(*pp)->blkno);
    }
    //printl("%d\n", b->blkno);
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
    printl("ide_print_blk: blk-%d, flags: ", b->blkno);
    if (b->flags & B_BUSY) //printl("B_BUSY ");
    if (b->flags & B_DIRTY) //printl("B_DIRTY");
    if (b->flags & B_VALID) //printl("B_VALID");
    printl("\n");
    int i,j;
    for (i = 0; i < BSIZE; i += 32){
        for (j = i; j < i + 32; j++){
            printl("%x ",b->data[j]);
        }
        printl("\n");
    }
}

struct buf buffer;  // used for test
void ide_test(){
    //printl("=== ide_test start ===\n");
    buffer.dev = 0;
    buffer.blkno= 10;
    buffer.flags = B_BUSY;
    ide_rw(&buffer);
    ide_print_blk(&buffer);
    int i = 0;
    for (i = 0; i < BSIZE; i++){
        buffer.data[i] = 1;
    }
    buffer.flags = B_BUSY|B_DIRTY;
    ide_rw(&buffer);

    buffer.flags = B_BUSY;
    ide_rw(&buffer);
    ide_print_blk(&buffer);
    //printl("=== ide_test end ===\n");

}
