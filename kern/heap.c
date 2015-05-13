/* heap.h
 * This file is modified form hurlex 
 * 内存堆分配， 实现kmalloc()和kfree()
 * 链表实现的侵入式内存管理
 */

#include <type.h> 
#include <heap.h>
#include <pmm.h>
#include <vmm.h>
#include <dbg.h>
#include <printk.h>

/* head node of chunk list */
static struct header *heap_first;
/* current heap address limit */
static uint32_t heap_max = HEAP_START;

/* 分配一个内存块, 从pmm_alloc_page 取得物理页，
 * 连续地映到start上  并不涉及对header的操作 */
static void alloc_chunk(uint32_t start, uint32_t len){
    while (start + len > heap_max){
        uint32_t page = pmm_alloc_page();
        map(pgd_kern, heap_max, page, PAGE_PRESENT|PAGE_WRITE);
        heap_max += PAGE_SIZE;
    }
}

/* free the **last chunk** of chunk list*/
/* 释放chunk， 然而只能释放链表中的最后一个chunk，
 * 只有释放最后一个时, heap_max 才能回退 */
static void free_chunk(struct header *chunk){
    if (chunk->prev == NULL){
        heap_first = NULL;
    } else {
        chunk->prev->next = NULL;
    }
    /* 当 chunk 的内存超过一页时， 持续释放 */
    while((heap_max - PAGE_SIZE) >= (uint32_t)chunk){
        heap_max -= PAGE_SIZE;
        uint32_t page;
        get_mapping(pgd_kern, heap_max, &page);
        unmap(pgd_kern, heap_max);
        pmm_free_page(page);
    }
}

/* 分割一个chunk */
static void split_chunk(struct header *chunk, uint32_t len){
    /*  内存大小比管理头还小的话就不必分割了 */
    if(chunk->length - len > sizeof(struct header)){
        //struct header *newchunk = (struct header *)(uint32_t)chunk + chunk->length);
        // 这里是 hurlex 中的一个错误，已改正
        struct header *newchunk = (struct header *)(uint32_t)chunk + len;
        newchunk->prev = chunk;
        newchunk->next = chunk->next;
        newchunk->allocated = 0;
        newchunk->length = chunk->length - len;

        chunk->next = newchunk;
        chunk->length = len;
    }

}

/* combine previous and next chunk with the specified chunk if they are not allocated */
/* 将chunk和其前后的空闲chunk合并 */
static void glue_chunk(struct header *chunk){
    if (chunk->next && chunk->next->allocated == 0){
        chunk->length += chunk->next->length;
        if (chunk->next->next){
            chunk->next->next->prev = chunk;
        }
        chunk->next = chunk->next->next;
    }
    if (chunk->prev && chunk->prev->allocated == 0){
        chunk->length += chunk->prev->length;
        if (chunk->prev->prev){
            chunk->prev->prev->next = chunk;
        }
        chunk->prev = chunk->prev->prev;
    }

    /* if this chunk(after glue) is the last one of list, free it */
    /* 是最后一个chunk的话直接释放 */
    if (chunk->next == 0){
        free_chunk(chunk);  //free_chunk() 只在这里被调用
    }
}

void heap_init(){
    heap_first = NULL;
}

void *kmalloc(uint32_t len){
    /* 包含header的长度 */
    len += sizeof(struct header);

    struct header *cur_header = heap_first;
    struct header *prev_header = NULL;

    while (cur_header){
        /* 在已有链表中寻找合适的chunk */
        if (cur_header->allocated == 0 && cur_header->length >= len){
            /* 有的话分割为合适的大小 */
            split_chunk(cur_header, len);
            cur_header->allocated = 1;
            /* 返回的时候不要包含 header */
            return (void *)((uint32_t)cur_header + sizeof(struct header));
        }
        prev_header = cur_header;
        cur_header = cur_header->next;
    }

    /* 没有合适的 chunk， 从 heap_max 处申请 chunk*/
    uint32_t chunk_start;
    /* 如果prev_header == NULL 说明是第一次申请， heap_max == HEAP_START 
     * 否则从prev_header末尾开始申请? TODO */
    if (prev_header){
        chunk_start = (uint32_t)prev_header + prev_header->length;
    }else{
        chunk_start = HEAP_START;
        heap_first = (struct header *)chunk_start;
    }

    alloc_chunk(chunk_start, len);
    cur_header = (struct header*)chunk_start;
    cur_header->prev = prev_header;
    cur_header->next = NULL;
    cur_header->allocated = 1;
    cur_header->length = len;

    if (prev_header){
        prev_header->next = cur_header;
    }

    return (void *)(chunk_start + sizeof(struct header));
}

void kfree(void *p){
    struct header *h = (struct header *)((uint32_t)p - sizeof(struct header));
    h->allocated = 0;

    /* glue 也包含了回收的动作 */
    glue_chunk(h);
}

/* 遍历header链表 */
void print_chunk_list(){
    struct header *cur = heap_first;
    while (cur){
        printk("cur->allocated: %d  ->base 0x%x  ->len %d\n",cur->allocated, (uint32_t)cur, cur->length);
        cur = cur->next;
    }
}

void heap_test(){
    void *addr1 = kmalloc(50);
    printk("kmalloc 50 byte in 0x%x\n", addr1);
    void *addr2 = kmalloc(500);
    printk("kmalloc 500 byte in 0x%x\n", addr2);
    void *addr3 = kmalloc(5000);
    printk("kmalloc 5000 byte in 0x%x\n", addr3);
    void *addr4 = kmalloc(5000);
    printk("kmalloc 5000 byte in 0x%x\n", addr4);
    void *addr5 = kmalloc(50000);
    printk("kmalloc 50000 byte in 0x%x\n", addr5);

    print_chunk_list();
    for (;;);
    //TODO 突然有点不明白

    printk("free mem in 0x%x\n",addr1);
    kfree(addr1);
    printk("free mem in 0x%x\n",addr2);
    kfree(addr2);
    printk("free mem in 0x%x\n",addr3);
    kfree(addr3);
    printk("free mem in 0x%x\n",addr4);
    kfree(addr4);
    printk("free mem in 0x%x\n",addr5);
    kfree(addr5);
}
