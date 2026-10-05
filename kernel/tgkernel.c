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

extern void test_user(void);

void enter_usr(void) {
    _uint32 ustk = tgkallocpage() + PAGESIZE;
    map_page(ustk-PAGESIZE, ustk-PAGESIZE, DEFAULT_USER);
    asm volatile (
        "movw $((4*8)|3), %%ax\n\t"
        "movw %%ax, %%ds\n\t"
        "movw %%ax, %%es\n\t"
        "movw %%ax, %%fs\n\t"
        "movw %%ax, %%gs\n\t"

        "movl %0, %%eax\n\t"

        "pushl $((4*8)|3)\n\t"
        "pushl %%eax\n\t"
        "pushfl\n\t"
        "pushl $((3*8)|3)\n\t"
        "pushl $test_user\n\t"

        "iret"
        :
        : "r"(ustk)
        : "eax", "memory"
    );
}

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
    ttyputchars(":- init out\n");
    asm volatile ("cli");
    char str[13]; its(123, str);
    ttyputcharsf(":- 123 in string form is '%'\n", str);
    _tgkinitidt();
    ttyputchars(":- init interrupts\n");
    _tgkinitgdt();
    ttyputchars(":- init gdt\n");
    vmeminit(mbi);
    ttyputchars(":- virtual mem init\n");
    map_page((_uint32)&test_user, (_uint32)&test_user, DEFAULT_USER);
    map_page((_uint32)&map_page, (_uint32)&map_page, DEFAULT_USER);
    map_page((_uint32)&ttyputchar, (_uint32)&ttyputchar, DEFAULT_USER);
    map_page((_uint32)&ttyputchars, (_uint32)&ttyputchars, DEFAULT_USER);
    map_page((_uint32)&ttyputcharsf, (_uint32)&ttyputcharsf, DEFAULT_USER);
    map_page((_uint32)&ttychangecolor, (_uint32)&ttychangecolor, DEFAULT_USER);
    map_page((_uint32)&fbclear, (_uint32)&fbclear, DEFAULT_USER);
    map_page((_uint32)&fbdrawpx, (_uint32)&fbdrawpx, DEFAULT_USER);
    map_page((_uint32)&fbfill, (_uint32)&fbfill, DEFAULT_USER);
    map_page((_uint32)&user_continue, (_uint32)&user_continue, DEFAULT_USER);
    map_page((_uint32)&__psh, (_uint32)&__psh, DEFAULT_USER);
    ttyputchars(":- mapped functions to be user-accesible\n");
    enter_usr();
    asm volatile ("sti");
}

void user_continue(void) {
    ttyputchars(":- userland init\n");
    ttyputchars(":- init done\n");
    fbclear();
    char ver[13];
    its(TAWAGOTO_VERSION, ver);
    ttyputcharsf("Welcome to Tawagoto (tgk v%)\n", ver);
    __psh((struct sys_info){"NCA", format("Tawagoto v%", ver)});
}
