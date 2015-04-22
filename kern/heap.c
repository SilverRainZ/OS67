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

static void alloc_chunk(uint32_t start, uint32_t len){
    while (start + len > heap_max){
        uint32_t page = pmm_alloc_page();
        map(pgd_kern, heap_max, page, PAGE_PRESENT|PAGE_WRITE);
        heap_max += PAGE_SIZE;
    }
}

/* free the **last chunk** of chunk list*/
static void free_chunk(struct header *chunk){
    if (chunk->prev == NULL){
        heap_first = NULL;
    } else {
        chunk->prev->next = NULL;
    }
    while((heap_max - PAGE_SIZE) >= (uint32_t)chunk){
        heap_max -= PAGE_SIZE;
        uint32_t page;
        get_mapping(pgd_kern, heap_max, &page);
        pmm_free_page(page);
    }
}

static void split_chunk(struct header *chunk, uint32_t len){
    if(chunk->length - len > sizeof(struct header)){
        //TODO ? 
        //struct header *newchunk = (struct header *)(uint32_t)chunk + chunk->length);
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
    if (chunk->next){
        free_chunk(chunk);
    }
}

void heap_init(){
    heap_first = NULL;
}

void *kmalloc(uint32_t len){
    len += sizeof(struct header);

    printk("required len: %d\n",len);

    struct header *cur_header = heap_first;
    struct header *prev_header = NULL;

    while (cur_header){
        if (cur_header->allocated == 0 && cur_header->length >= len){

            printk("found compatiable chunk: 0x%x\n",(uint32_t)cur_header);

            split_chunk(cur_header, len);
            cur_header->allocated = 1;
            return (void *)((uint32_t)cur_header + sizeof(struct header));
        }

        printk("current chunk: 0x%x\n",(uint32_t)cur_header);

        prev_header = cur_header;
        cur_header = cur_header->next;
    }

    uint32_t chunk_start;

    if (prev_header){
        chunk_start = (uint32_t)prev_header + prev_header->length;
    }else{
        chunk_start = HEAP_START;
        heap_first = (struct header *)chunk_start;
    }

    printk("new chunk start: 0x%x\n",(uint32_t)chunk_start);

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

    glue_chunk(h);
}

void heap_test(){
    void *addr1 = kmalloc(50);
    printk("kmalloc 50 byte in 0x%x\n", addr1);
    void *addr2 = kmalloc(500);
    printk("kmalloc 500 byte in 0x%x\n", addr2);
    return;
    void *addr3 = kmalloc(5000);
    printk("kmalloc 5000 byte in 0x%x\n", addr3);
    void *addr4 = kmalloc(5000);
    printk("kmalloc 5000 byte in 0x%x\n", addr4);
    void *addr5 = kmalloc(50000);
    printk("kmalloc 50000 byte in 0x%x\n", addr5);

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

