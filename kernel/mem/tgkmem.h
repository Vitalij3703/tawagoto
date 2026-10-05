#ifndef _TGKMEM
#define _TGKMEM
#include "boot/multiboot.h"
#include "tglib/tgtype.h"

// the size of a singular page (maybe i'm not so bad at spelling)
#define PAGESIZE 4096

#define PAGEFAIL -1

static volatile _uint8* bitmap;
static _uint32 tpages;
static _uint32 lalloc;

// get byte index
#define gbi(i)  (i/8)

// get bit index
#define gbii(i) (i%8)

// get page from physical addr
#define gpfpa(addr) (addr/4096)

// get physical addr from page
#define gpafp(page) (page*4096)

// make page used
#define mpu(p, i) bitmap[i] = bitmap[i] | (1 << p)

// make page free
#define mpf(p, i) bitmap[i] = bitmap[i] &~ (1 << p)

// is page used
#define ipu(p, i) ((bitmap[i] & (1 << p)) != 0)

// is page free
#define ipf(p, i) (!ipu(p, i))

// round address down
#define rad(addr) (addr & ~(PAGESIZE-1))

// round address up
#define rau(addr) ((addr)+(PAGESIZE-1) & ~(PAGESIZE-1))


void tgkmeminit(multiboot_info_t* info);

_uint32 tgkallocpage();

void tgkfreepage(_uint32 loc);

void tgkallocpages(_uint size, _uint32* buf);

void tgkfreepages(_uint32 loc, _uint size);

void* pmalloc(_uint size);

#endif