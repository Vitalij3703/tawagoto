#ifndef _TGIDT
#define _TGIDT
#include "tglib/tgtype.h"

// all the exceptions
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);
#define PIC1 0x20
#define PIC2 0xA0
#define PIC1C PIC1
#define PIC1D (PIC1+1)
#define PIC2C PIC2
#define PIC2D (PIC2+1)

struct idt_entry {
    _uint16 handler_low;
    _uint16 selector;
    _uint8  zero;
    _uint8  t_attr; // type attributes
    _uint16 handler_high;
} __attribute__((packed));

struct idt_ptr {
    _uint16 limit;
    _uint32 base;
} __attribute__((packed));

#define lidt(ptr) asm volatile ("lidt %0" : : "m"(ptr))

struct iframe {
    _uint32 edi;
    _uint32 esi;
    _uint32 ebp;
    _uint32 esp;
    _uint32 ebx;
    _uint32 edx;
    _uint32 ecx;
    _uint32 eax;
    _uint32 inum; // interrupt number
    _uint32 errc; // error code
    _uint32 eip;
    _uint32 cs;
    _uint32 eflags;
    _uint32 user_esp;
    _uint16 user_ss;
} __attribute__((packed));

void _tgkinitidt();

void _tgkexception(struct iframe *f);

void _tgkinterrupt(struct iframe *f);

// moved inb and outb to tgkernel.h

#endif