// tawagoto kernel
#ifndef __TGK
#define __TGK
#define _ATR_DEFAULT 0x07
#include "tglib/tgtype.h"
#include "boot/multiboot.h"

void _tgkmain(_size32 magic, multiboot_info_t* mbi); // the kernel main
void _tgkpanic(char *reason); // kernel panic
void __tgkhang__(void);

static inline _uint8 inb(_uint16 p) {
    _uint8 r;
    asm volatile ("inb %1, %0" : "=a"(r) : "Nd"(p));
    return r;
}

static inline void outb(_uint16 p, _uint8 v) {
    asm volatile ("outb %0, %1" : : "a"(v), "Nd"(p));
}

#endif