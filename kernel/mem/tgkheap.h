#ifndef _TGKHEAP
#define _TGKHEAP
#include "mem/tgkmem.h"
#include "tglib/tgtype.h"

struct heapblock {
    struct heapblock* next;
    _uint32 size;
    _uint8 free;
} heapblock;

void* kmalloc(_uint rs);
void kmfree(void* amem);

#endif