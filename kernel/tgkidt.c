#include "tgkidt.h"
#include "tgkernel.h"
//#include "tgkout.h"
#include "tty/tty.h"
#include "tglib/tgtconv.h"
#include "driver/key/tgkkey.h"
#include "driver/timer/timer.h"

void _tgkexception(struct iframe *f) {
    const char* enames[32] = {
        "Division by 0", // 1
        "Debug", // 2
        "Non-maskable interrupt", //3
        "Breakpoint", //4
        "Overflow", //5
        "Bound range exceeded", //6
        "Invalid opcode", //7
        "Device nit available", //8
        "Double fault", //9
        "Coprocessor segment overrun", //10
        "Invalid TTS", //11
        "Segment not present", //12
        "Stack-segment fault", //13
        "General protection fault", //14
        "Page fault", //15
        "16", //16
        "x87 floating-point exception", // 17
        "Alignment check", //18
        "Machine check", //19
        "SIMD floating-point exception", //20
        "Virtualization exception", // 21
        "Control protection exception", //22
        "23", //23
        "24", //24
        "25", //25
        "26", //26
        "27", //27
        "28", //28
        "Hypervisor Injection exception", //29
        "VMM communication exception", //30
        "Security exception", //31
        "32" 
    };
    char eips[13];
    xts(f->eip, eips);
    char err[13];
    xts(f->errc, err);
    ttyputcharsf("\n:- CPU EXCEPTION\n:- name %\n:- eip %\n:- err code %\n\n", enames[f->inum], eips, err);
    
}
volatile _uint32 test = 0;
void _tgkinterrupt(struct iframe *f) {
    if(f->inum < 32) {
        _tgkexception(f);
        return;
    }
    else /*if(f->inum < 48)*/ {
        if(f->inum == 32) {
            test++;
            outb(0x20, 0x20);
        }
        
        if(f->inum == 33) 
            //_tgkkpk(f->inum);
            //_tgkpanic("keyboard works yayyyyy");
            outb(0x20, 0x20);
    }

}

struct idt_entry cientry(_uint32 addr) {
    struct idt_entry f;
    f.handler_low = addr & 0xFFFF;
    f.selector = 0x08;
    f.zero = 0;
    f.t_attr = 0x8E;
    f.handler_high = addr >> 16;
    return f;
} 

void _tgkinitidt() {
    asm volatile("cli");
    static struct idt_entry f[256];
    f[0] = cientry((_uint32)isr0);
    f[1] = cientry((_uint32)isr1);
    f[2] = cientry((_uint32)isr2);
    f[3] = cientry((_uint32)isr3);
    f[4] = cientry((_uint32)isr4);
    f[5] = cientry((_uint32)isr5);
    f[6] = cientry((_uint32)isr6);
    f[7] = cientry((_uint32)isr7);
    f[8] = cientry((_uint32)isr8);
    f[9] = cientry((_uint32)isr9);
    f[10] = cientry((_uint32)isr10);
    f[11] = cientry((_uint32)isr11);
    f[12] = cientry((_uint32)isr12);
    f[13] = cientry((_uint32)isr13);
    f[14] = cientry((_uint32)isr14);
    f[15] = cientry((_uint32)isr15);
    f[16] = cientry((_uint32)isr16);
    f[17] = cientry((_uint32)isr17);
    f[18] = cientry((_uint32)isr18);
    f[19] = cientry((_uint32)isr19);
    f[20] = cientry((_uint32)isr20);
    f[21] = cientry((_uint32)isr21);
    f[22] = cientry((_uint32)isr22);
    f[23] = cientry((_uint32)isr23);
    f[24] = cientry((_uint32)isr24);
    f[25] = cientry((_uint32)isr25);
    f[26] = cientry((_uint32)isr26);
    f[27] = cientry((_uint32)isr27);
    f[28] = cientry((_uint32)isr28);
    f[29] = cientry((_uint32)isr29);
    f[30] = cientry((_uint32)isr30);
    f[31] = cientry((_uint32)isr31);
    f[32] = cientry((_uint32)irq0);
    f[33] = cientry((_uint32)irq1);
    f[34] = cientry((_uint32)irq2);
    f[35] = cientry((_uint32)irq3);
    f[36] = cientry((_uint32)irq4);
    f[37] = cientry((_uint32)irq5);
    f[38] = cientry((_uint32)irq6);
    f[39] = cientry((_uint32)irq7);
    f[40] = cientry((_uint32)irq8);
    f[41] = cientry((_uint32)irq9);
    f[42] = cientry((_uint32)irq10);
    f[43] = cientry((_uint32)irq11);
    f[44] = cientry((_uint32)irq12);
    f[45] = cientry((_uint32)irq13);
    f[46] = cientry((_uint32)irq14);
    f[47] = cientry((_uint32)irq15);
    _uint8 p1 = inb(PIC1D);
    _uint8 p2 = inb(PIC2D);
    outb(PIC1C, 0x11);
    outb(PIC2C, 0x11);
    outb(PIC1D, 0x20);
    outb(PIC2D, 0x28);
    outb(PIC1D, 0x04);
    outb(PIC2D, 0x02);
    outb(PIC1D, 0x01);
    outb(PIC2D, 0x01);
    outb(PIC1D, p1);
    outb(PIC2D, p2);
    outb(PIC1D, 0xFC);
    outb(PIC2D, 0xFF);
    unsigned div = 1193182 / 1000;
   outb(0x43, 0x34);
   outb(0x40, div & 0xFF);
   outb(0x40, div >> 8);
    static struct idt_ptr ptr;
    ptr.limit = sizeof(f) - 1;
    ptr.base = (_uint32)&f;
    lidt(ptr);
    _uint8 mask = inb(PIC1D);
    mask = mask & ~0x03;
    outb(PIC1D, mask);
   _tgkekd();
   tinit(&test);
}