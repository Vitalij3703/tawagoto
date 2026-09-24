
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
    for (int i = 0; i < 1024; i++) {
        _uint32* page_table;
        for (int ii = 0; ii < 1024; ii++) { 
            page_table = (_uint32*)tgkallocpage();
            page_table[ii] = entry(i*PAGESIZE, 0x003);
        }
        page_direc[i] = entry(page_table, 0x003);
    }
    
    enablepaging(page_direc);
}

