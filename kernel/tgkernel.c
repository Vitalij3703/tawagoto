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

void t2f() {
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
    //__psh((struct sys_info){"N/A", "Tawagoto DEV"});
    map_page((_uint32)&test_user, (_uint32)&test_user, DEFAULT_USER);
    map_page((_uint32)&map_page, (_uint32)&map_page, DEFAULT_USER);
    ttyputchars(":- mapped functions to be user-accesible\n");
    //ttyputchars("\nif you see a genereal protection fault, usermode is working\n");
    //enter_usr();
    struct sys_info* sysinfo = kmalloc(sizeof(struct sys_info));
    sysinfo->mem = "N/A ";
    sysinfo->os = "Tawagoto DEV0";
    struct thread* t = initial_thread_create(0, NULL);
    char tid[13];
    char state[13];
    char esp[13];
    char kstack[13];
    char kstackt[13];
    char ieip[13];
    char arg[13];
    char time[13];
    char rs[13];
    char rst[13];
    its(t->tid, tid);
    its(t->state, state);
    its(t->esp, esp);
    its(t->kstack, kstack);
    its(t->kstackt, kstackt);
    its(t->ieip, ieip);
    its((int)t->arg, arg);
    its(t->time, time);
    its((_uint32)stack_bottom, rs);
    its((_uint32)stack_top, rst);
    ttyputcharsf("tid: %\nstate: %\nesp: %\nkstack: %\nkstackt: %\nieip: %\n arg: %\ntrue kstack: %\ntrue kstackt: %\n",tid,state,esp,kstack,kstackt,ieip,arg,rs,rst);
    asm volatile ("sti");
    ttyputchars(":- thread p1 test\n");
    ttyputchars(":- thread p2 test\n");
    struct thread* t2 = thread_create((_uint32)t2f, NULL, NULL);
    ttyputchars(":- initializing psh (q to exit)\n");
    struct thread* psh = thread_create((_uint32)__psh, (void*)sysinfo, NULL);
    make_thread_keyowning(psh);
    char tid1[13];
    char state1[13];
    char esp1[13];
    char kstack1[13];
    char kstackt1[13];
    char ieip1[13];
    char arg1[13];
    char time1[13];
    char rs1[13];
    char rst1[13];
    its(psh->tid, tid1);
    its(psh->state, state1);
    its(psh->esp, esp1);
    its(psh->kstack, kstack1);
    its(psh->kstackt, kstackt1);
    its(psh->ieip, ieip1);
    its((int)psh->arg, arg1);
    its(psh->time, time1);
    its((_uint32)stack_bottom, rs1);
    its((_uint32)stack_top, rst1);
    ttyputcharsf("tid: %\nstate: %\nesp: %\nkstack: %\nkstackt: %\nieip: %\n arg: %\ntrue kstack: %\ntrue kstackt: %\n",tid1,state1,esp1,kstack1,kstackt1,ieip1,arg1,rs1,rst1);
    ttyputchars(":- done\n");
}