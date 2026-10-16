#include "mem/tgkheap.h"
#include "mem/tgkmem.h"
#include "tglib/tgtype.h"

#define ALIGN      (8)
#define ALIGN_8(n) ((n)+(ALIGN-1) & ~(ALIGN-1))
#define HEADSIZE   (ALIGN_8(sizeof(struct heapblock)))

struct heapblock* hhead = NULL;
struct heapblock* htail = NULL;

// not a good name
#define HEAP_SUCC 0
#define HEAP_FAIL 1

int heap_grow() {
    _uint32 page = tgkallocpage();
    if(page == PAGEFAIL) return HEAP_FAIL;
    struct heapblock* block = (struct heapblock*)(page);
    block->size = PAGESIZE-HEADSIZE;
    block->free = 1;
    block->next = NULL;
    block->magic = MEMMAGIC;
    if(hhead == NULL) { // assuming if it has no head, it has no tail... will work unless the code does some stupid shit
        hhead = block;
        htail = block;
    } else {
        htail->next = block;
        htail = block;
    }
    return HEAP_SUCC;
}

#define MINAHH (8)

// no ai will name things this badly
void split_ahh(struct heapblock* block, _uint rs) {
    _uint left = block->size - rs;
    if(left<MINAHH+HEADSIZE) return; // just use the whole block
    struct heapblock* new = (struct heapblock*)(((char*)block)+HEADSIZE+rs);
    new->size = left - HEADSIZE;
    new->free = 1;
    new->magic = MEMMAGIC;
    new->next = block->next;
    block->size = rs;
    block->next = new;
    if(block == htail) // pointer comparasions be like
        htail = new;
}

void* kmalloc(_uint rss) {
    if(rss == 0) return NULL;
    _uint rs = ALIGN_8(rss);
    if(rs>PAGESIZE-HEADSIZE) return NULL;
    search: //scary
    struct heapblock* cblok = hhead;
    while(cblok != NULL) {
        if(cblok->magic != MEMMAGIC)
            return NULL;
        if(cblok->free && cblok->size >= rs )
            break;
        cblok = cblok->next;
    }
    if(cblok == NULL) {
        if(heap_grow())
            return NULL;
        goto search;
    }
    split_ahh(cblok, rs);
    cblok->free = 0;
    return cblok+1;
}

// can merge incase you dont get it
int can_reverse_mitosis(struct heapblock* a, struct heapblock* b) {
    if(a == NULL || b == NULL) return 0;
    if(a->free == 0 || b->free == 0) return 0;
    if((char*)a+HEADSIZE+a->size == (char*)b) return 1;
    return 0;
}

void coalase() {
    struct heapblock* cblok = hhead;
    while(cblok != NULL && cblok->next != NULL) {
        if(can_reverse_mitosis(cblok, cblok->next)) {
            cblok->size += HEADSIZE + cblok->next->size;
            if(cblok->next == htail) 
                htail = cblok;
            cblok->next = cblok->next->next; // wait until you see cblok->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next;
            
        } else 
            cblok = cblok->next;
    }
}

int kmfree(void* amem) {
    if(amem == NULL) return 0;
    struct heapblock* block = ((struct heapblock*)amem)-1;
    if(block->magic == MEMMAGIC && block->free == 0) {
        block->free = 1;
        coalase();
        return 0;
    }
    return BAD_PTR;
}