
#include "mem/tgkvmem.h"
#include "mem/tgkmem.h"
//define entry(addr, flags) ((addr << 12) | flags)
//void vmeminit(multiboot_info_t* info);
//_uint32 mmap(_uint bytes, _uint flags);
#include "boot/multiboot.h"
#include "tglib/tgtype.h"

extern void enablepaging(_uint32* pd);

void vmeminit(multiboot_info_t* info) {
    page_direc = (_uint32*)tgkallocpage();
    int flag = DEFAULT_ROOT;
    for (int i = 0; i < 1024; i++) {
        _uint32* page_table = (_uint32*)tgkallocpage();
        for (int ii = 0; ii < 1024; ii++) {
            if((i*1024+ii)*PAGESIZE > 0x3FFFFFFF) flag = DEFAULT_USER;
            page_table[ii] = entry((i*1024+ii)*PAGESIZE, flag);
        }
        page_direc[i] = entry(page_table, flag);
    }
    
    enablepaging(page_direc);
}

_uint32 map_page(_uint32 paddr, _uint32 vaddr, _uint16 flags) {
    _uint32* direc = &page_direc[vaddr >> 22];
    if(!(*direc & (1 << 31))){
        _uint32* nap = (_uint32*)tgkallocpage();
        direc = nap;
        page_direc[vaddr>>22] = entry(nap, flags);
    }
    _uint32* index = &(direc[(vaddr>>12) & 0x3FF]);
    *index = entry(paddr, flags);
    asm volatile ("invlpg %0\n\t"::"m"(vaddr));
    return vaddr;
}