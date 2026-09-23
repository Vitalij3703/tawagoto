// tawagoto kernel
// see definitions in tgkernel.h

#define __MB_VITALIJ3703
#include "tgkernel.h"
#include "tglib/tgstr.h"
#include "tglib/tgtype.h"
#include "tglib/tgtconv.h"
//#include "tgkout.h"
#include "tgkgdt.h"
#include "tgkidt.h"
#include "boot/multiboot.h"
#include "mem/tgkmem.h"
#include "mem/tgkvmem.h"
#include "bin/psh/psh.h"
#include "tty/tty.h"

_uint8 inb(_uint16 p) {
    _uint8 r;
    asm volatile ("inb %1, %0" : "=a"(r) : "Nd"(p));
    return r;
}

void outb(_uint16 p, _uint8 v) {
    asm volatile ("outb %0, %1" : : "a"(v), "Nd"(p));
}

// please do not call this function
__attribute__((noreturn))
void __tgkhang__(void) {
    asm volatile ("cli");
    while(1)
        asm volatile ("hlt");
}

void _tgkpanic(char *reason) {
    ttychangecolor(0x0000FF, 0xFFFFFF);
    fbclear();
    ttyputchars(":- TG KERNEL PANIC\n");
    ttyputchars("\tYour computer has run into a problem, and we won't fix it... \n\tunless, you report it to the dev and say 'pwetty pwease'\n\n\tPanic reason:\n\t");
    if(reason)
        ttyputcharsf("\t%", reason);
    else ttyputchars("\t< No reason provided >");
    __tgkhang__();
}

void _tgkmain(_size32 magic, multiboot_info_t* mbi) {
    if (magic != 0x2BADB002) return;
    asm volatile ("cli");
    //_tgkprints("tawagoto kernel succ. w nights spider\0", _ATR_DEFAULT);
    tgkmeminit(mbi);
    _uint32 page = tgkallocpage();
    tgkfreepage(page);
    fbinit(mbi);
    char ver[13];
    its(TAWAGOTO_VERSION, ver);
    char mem[13];
    its((mbi->mem_lower + mbi->mem_upper)/1024, mem);
    ttyputcharsf("tawagoto kernel v%\n\tby solez\n", ver);
    ttyputcharsf(":- mem %MiB\n", mem);
    ttyputchars(":- init tty\n");
    vmeminit(mbi);
    ttyputchars(":- virtual mem init\n");
    asm volatile ("cli");
    char str[13]; its(123, str);
    ttyputcharsf(":- 123 in string form is '%'\n", str);
    _tgkinitidt();
    ttyputchars(":- init interrupts\n");
    _tgkinitgdt();
    ttyputchars(":- init gdt\n");
    ttyputchars(":- init done\n");
    asm volatile ("sti");
    fbclear();
    ttyputcharsf("Welcome to Tawagoto (tgk v%)\n", ver);
    __psh((struct sys_info){mem, format("Tawagoto v%", ver)});
    
}
