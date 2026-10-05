#ifndef _TGKVMEM
#define _TGKVMEM
#include "tgkmem.h"
#include "tglib/tgtype.h"
#include "boot/multiboot.h"

#define DEFAULT_USER 0x007
#define DEFAULT_ROOT 0x003

#define PRESENT 0x001
#define WRITABLE 0x002
#define USER 0x004
#define WRITE_TRU 0x008
#define NO_CACHE 0x010
#define ACCESSED 0x020
#define DIRTY 0x040
#define PAT 0x080
#define GLOBAL 0x100
#define AVL1 0x200
#define AVL2 0x400
#define AVL3 0x800

static _uint32* page_direc;

#define entry(addr, flags) (((_uint32)addr & ~0xFFF) | flags)

void vmeminit(multiboot_info_t* info);

_uint32 map_page(_uint32 paddr, _uint32 vaddr, _uint16 flags);

#endif