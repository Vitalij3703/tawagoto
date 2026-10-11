#ifndef _TGKHEAP
#define _TGKHEAP
#include "mem/tgkmem.h"
#include "tglib/tgtype.h"

// 00ILOVEC
#define MEMMAGIC (0x0011085C)

static struct heapblock {
    struct heapblock* next;
    _uint32 size;
    _uint8 free;
    _uint32 magic;
} heapblock __attribute__((aligned(4096)));

void* kmalloc(_uint rs);

#define BAD_PTR (0x11BADBAD)

void kmfree(void* amem);

#endif