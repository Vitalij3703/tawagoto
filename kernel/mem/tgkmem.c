#include "tgkmem.h"
#include "boot/multiboot.h"
//#include "tgkout.h"
#include "tglib/tgtype.h"
#include "tglib/tgtconv.h"

extern _uint8 tgkstart;
extern _uint8 tgkend;

void mpsu(_uint32 base, _uint32 len) {
    _uint32 addr = base;
    while(addr < len) {
        mpu(gbii(gpfpa(addr)), gbi(gpfpa(addr)));
        addr+=PAGESIZE;
    }
}

_uint bmsize;

void tgkmeminit(multiboot_info_t* info) {
    if (!(info->flags & (1 << 6))) return;
    _uint32 max_addr = 0;
    struct mmap_entry* map = (struct mmap_entry*)(info->mmap_addr);
    _uint8* end = (_uint8*)((_uint32)map + info->mmap_length);
    for (_uint8* q = (_uint8*)map; q < end;) {
        struct mmap_entry* centry = (struct mmap_entry*)q;
        _uint32 entrye = rad(centry->baddr) + rau(centry->len);
        if(entrye > max_addr)
            max_addr = entrye;
        q += centry->size + sizeof(centry->size);
    }
    tpages = max_addr/PAGESIZE;
    bmsize = (tpages+7)/8;
    _uint32 bms = rau((_uint32)&tgkend); // bitmap start address
    bitmap = (_uint8*)bms;
    for (_uint i = 0; i < bmsize; i++)
        bitmap[i] = 0xFF;
    for (_uint8* q = (_uint8*)map; q < end;) {
        struct mmap_entry* centry = (struct mmap_entry*)q;
        if(centry->type == 1) {
            _uint32 a = rau(centry->baddr);
            _uint32 top = rad(centry->baddr + centry->len);
            for (; a+PAGESIZE <= top; a+=PAGESIZE) {
                _uint32 pn = gpfpa(a);
                mpf(gbii(pn), gbi(pn));
            }
        }
        q += centry->size + sizeof(centry->size);
    }
    mpsu(0, 0x100000);
    mpsu(rad((_uint32)&tgkstart), rau((_uint32)&tgkend)); // tawagoto: youre reserving memory for me?? hmph it's not like i will initialize that bitmap for you
                                                          // what the fuck am i doing
    mpsu((_uint32)bitmap, rau((_uint32)bitmap+bmsize));
}

_uint32 tgkallocpage() {
    for (_uint32 i = 0; i < bmsize; i++) {
        if(bitmap[i] == 0xFF) continue;
        for (_uint8 ii = 0; ii < 8; ii++) {
            if(ipf(ii, i)) {
                mpu(ii, i);
                _uint pn = i*8+ii;
                return gpafp(pn);
            }
        }
    }
    return PAGEFAIL;
}

void tgkfreepage(_uint32 loc) {
    if(loc==PAGEFAIL) return;
    _uint32 pn = gpfpa(loc);
    mpf(gbii(pn), gbi(pn));
}

void tgkallocpages(_uint size, _uint32* buf) {
    for (int i = 0; i < size; i++) {
        buf[i] = tgkallocpage();
    }
}

void tgkfreepages(_uint32 loc, _uint size) {
    for (int i = 1; i < size+1; i++) {
        tgkfreepage(loc + (PAGESIZE*i));
    }
}

// physical memory allocate
void* pmalloc(_uint size) {
    _uint   reqpage = rau(size)/PAGESIZE;
    _uint32 pages[reqpage];
    tgkallocpages(reqpage, pages);
    return (void*)pages;
}

void pmfree(_uint32 addr, _uint size) {
    _uint reqpage = rau(size)/PAGESIZE;
    tgkfreepages(addr, reqpage);
}