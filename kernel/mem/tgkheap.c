#include "mem/tgkheap.h"
#include "mem/tgkmem.h"
#include "tglib/tgtype.h"

void* kmalloc(_uint rs) {
    struct heapblock* block = (struct heapblock*)tgkallocpage(); 
    block->next = (struct heapblock*)NULL;
    block->size = PAGESIZE-sizeof(struct heapblock);
    block->free = 1;
    if(block->size >= rs + sizeof(block)+8) {
        struct heapblock* sblock = (struct heapblock*)tgkallocpage();
        sblock->size = block->size - rs - sizeof(heapblock);
        block->size = rs;
        block->free = 0;
        block->next = sblock;
    } else
        block->free = 0;
    return (void*)(block+1);
}

void kmfree(void* amem) {
    if(amem == NULL) return;
    struct heapblock* block = ((struct heapblock*)amem)-1;
    block->free = 1;
}