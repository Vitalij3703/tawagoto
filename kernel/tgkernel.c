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
#include "proc/proc.h"
#include "mem/tgkheap.h"

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



// please do not call this function
__attribute__((noreturn))
void __tgkhang__(void) {
    asm volatile ("cli");
    while(1)
        asm volatile ("hlt");
}

void __tgkbeepst(_uint f) {
    _uint d = 1193180/f;
    outb(0x43, 0xB6);
    outb(0x42, d & 0xFF);
    outb(0x42, (d>>8)&0xFF);
    _uint8 t = inb(0x61);
    if((t&3)!=3) outb(0x61, t | 3);
}

void __tgkbeepno() {
    outb(0x61, inb(0x61)&0xFC);
}

#define CAPS_WAIT while(inb(0x64)&2)

void caps_toggle(int state) {
    CAPS_WAIT;
    outb(0x60, 0xED);
    CAPS_WAIT;
    outb(0x60,state?0x04:0x00);
}

void _tgkpanic(char *reason) {
    asm volatile ("cli");
    __tgkbeepst(1200);
    caps_toggle(1);
    for(volatile _uint i = 0; i<1024*1024; i++);
    __tgkbeepno();
    caps_toggle(0);
    ttychangecolor(0x0000FF, 0xFFFFFF);
    fbclear();
    ttyputchars(":- TG KERNEL PANIC\n");
    ttyputchars("\tYour computer has run into a problem, and we won't fix it... \n\tunless, you report it to the dev and say 'pwetty pwease'\n\n\tPanic reason:\n\t");
    if(reason)
        ttyputcharsf("\t%", reason);
    else ttyputchars("\t< No reason provided >");
    __tgkhang__();
}

void test_thread() {
    while(1) {
        __tgkbeepst(3000);
        caps_toggle(1);
        __tgkbeepno();
        caps_toggle(0);
        fbdrawpx(1, 1, 0xFFFFFF);
    }
}

void _tgkmain(_size32 magic, multiboot_info_t* mbi) {
    if (magic != 0x2BADB002) return;
    asm volatile ("cli");
    //_tgkprints("tawagoto kernel succ. w nights spider\0", _ATR_DEFAULT);
    tgkmeminit(mbi);
    _uint32 page = tgkallocpage();
    tgkfreepage(page);
    fbinit(mbi);
    char fbaddr[13];
    its(mbi->framebuffer_addr, fbaddr);
    char ver[13];
    its(TAWAGOTO_VERSION, ver);
    char mem[13];
    its((mbi->mem_lower + mbi->mem_upper)/1024, mem);
    ttyputcharsf("tawagoto kernel v%\n\tby solez\n", ver);
    ttyputcharsf(":- fb addr %\n", fbaddr);
    ttyputcharsf(":- mem % MiB\n", mem);
    ttyputchars(":- initialized framebuffer output\n");
    _tgkinitidt();
    ttyputchars(":- initialized interrupts\n");
    _tgkinitgdt();
    ttyputchars(":- initialized the gdt\n");
    vmeminit(mbi);
    ttyputchars(":- initialized virtual memory\n");

    struct sys_info* sysinfo = kmalloc(sizeof(struct sys_info));
    sysinfo->mem = format("% ", mem);
    sysinfo->os = format("Tawagoto DEV %", ver);

    initial_thread_create(0, NULL);
    ttyputchars(":- initialized boot thread\n");
    thread_create((_uint32)test_thread, NULL, NULL);
    ttyputchars(":- initialized test thread\n");
    //thread_create((_uint32)reap_threads, NULL, NULL);
    ttyputchars(":- initialized reaper thread\n");
    struct thread* psh = thread_create((_uint32)__psh, (void*)sysinfo, NULL);
    make_thread_keyowning(psh);
    ttyputchars(":- initialized psh (q to exit)\n");
    asm volatile ("sti");
    ttyputchars(":- done\n");
}