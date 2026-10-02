#ifndef _TGKVMEM
#define _TGKVMEM
#include "tgkmem.h"
#include "tglib/tgtype.h"
#include "boot/multiboot.h"

#define DEFAULT_USER 0x007
#define DEFAULT_ROOT 0x003

static _uint32* page_direc;

#define entry(addr, flags) (((_uint32)addr & ~0xFFF) | flags)

void vmeminit(multiboot_info_t* info);

_uint32 map_page(_uint32 paddr, _uint32 vaddr, _uint16 flags);

#endif