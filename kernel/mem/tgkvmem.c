
#include "mem/tgkvmem.h"
#include "mem/tgkmem.h"
//define entry(addr, flags) ((addr << 12) | flags)
//void vmeminit(multiboot_info_t* info);
//_uint32 mmap(_uint bytes, _uint flags);
#include "boot/multiboot.h"
#include "tglib/tgtype.h"
#include "tglib/tgstr.h"
#include "tty/tty.h"
#include "tgkernel.h"
#include "tglib/tgtconv.h"

extern void enablepaging(_uint32* pd);

_uint32* page_direc;

#define FB_MOVE_ADDR (0x80000000)
#define USERSPACE_ADDR (0x40000000)

void vmeminit(multiboot_info_t* info) {
    page_direc = (_uint32*)tgkallocpage();
    if(page_direc == PAGEFAIL)
        _tgkpanic("Failed to initialize virtual memory.\nReboot.");
    int flag = DEFAULT_ROOT;
    for (_uint32 i = 0; i < 1024; i++) {
        _uint32* page_table = (_uint32*)tgkallocpage();
        for (_uint32 ii = 0; ii < 1024; ii++) {
            if((i*1024+ii)*PAGESIZE >= USERSPACE_ADDR
                && (i*1024+ii)*PAGESIZE <= FB_MOVE_ADDR) flag = DEFAULT_USER;
            else flag = DEFAULT_ROOT;
            page_table[ii] = entry((i*1024+ii)*PAGESIZE, flag);
        }
        page_direc[i] = entry(page_table, flag);
    }

    if((_uint32)fb < FB_MOVE_ADDR) {
        if(rau(fbend) + FB_MOVE_ADDR < FB_MOVE_ADDR)
            _tgkpanic("Framebuffer overflow, get a smaller display.\nWhere did you even get such a large display?");
        _uint32 re = rau(fbend);
        _uint32 rs = rad((_uint32)fb);
        _uint32 rqp = (re - rs) / PAGESIZE;
        char frq[13];
        its(rqp, frq);
        ttyputcharsf(":- pages requested by fb %\n", frq);
        for(_uint i = 0; i < rqp; i++)
            map_page(rs+i*PAGESIZE, rs+i*PAGESIZE+FB_MOVE_ADDR, DEFAULT_ROOT);
        fb = fb + FB_MOVE_ADDR;
    } else
        ttyputchars(":- framebuffer move not needed\n");
    
    enablepaging(page_direc);
}

_uint32 map_page(_uint32 paddr, _uint32 vaddr, _uint16 flags) {
    _uint32* direc = &page_direc[vaddr >> 22];
    _uint32* table = (_uint32*)(*direc & ~0xFFF);
    if(!(*direc & 1)){
        _uint32* nap = (_uint32*)tgkallocpage();
        memset((_uint8*)nap, 0, PAGESIZE);
        table = nap;
        page_direc[vaddr>>22] = entry(nap, flags);
    }
    if (flags & USER)
        *direc |= USER;
    _uint32* index = &(table[(vaddr>>12) & 0x3FF]);
    *index = entry(paddr, flags);
    asm volatile ("invlpg (%0)\n\t"::"r"(vaddr):"memory");
    return vaddr;
}