#include "tgkgdt.h"
#include "tglib/tgtype.h"
#include "tglib/tgstr.h"

#define GDTE 6

extern __uint32 stack_top;
extern void flush_tss(void);

struct tss_entry {
	_uint32 prev_tss;
	_uint32 esp0;
	_uint32 ss0;
	_uint32 esp1;
	_uint32 ss1;
	_uint32 esp2;
	_uint32 ss2;
	_uint32 cr3;
	_uint32 eip;
	_uint32 eflags;
	_uint32 eax;
	_uint32 ecx;
	_uint32 edx;
	_uint32 ebx;
	_uint32 esp;
	_uint32 ebp;
	_uint32 esi;
	_uint32 edi;
	_uint32 es;
	_uint32 cs;
	_uint32 ss;
	_uint32 ds;
	_uint32 fs;
	_uint32 gs;
	_uint32 ldt;
	_uint16 trap;
	_uint16 iomap_base;
} __attribute__((packed));

struct tss_entry _tsse; // these horrible names prove my code isnt ai (and using tgtype.h instead of stdint.h)

void __wtss(struct gdt_entry* e) {
    _uint32 b = (_uint32)&_tsse;
    _uint32 l = sizeof(_tsse) - 1;
    e->limit_low = l;
    e->base_low = b;
    e->base_high = (b >> 24) & 0xFF;
    e->base_mid = (b >> 16) & 0xFF;
    e->access = 0x89;
    e->gran = 0x0;
    memset((__uint8*)&_tsse, 0, sizeof(_tsse));
    _tsse.ss0 = 0x10;
    _tsse.esp0 = (_uint32)&stack_top;
}

void __sks(_uint32 s) {
    _tsse.esp0 = s;
}

void _tgkinitgdt() {
    asm volatile("cli");
    static struct gdt_entry gdt[GDTE];
    struct gdt_entry null;
    null.limit_low = 0x0000;
    null.base_low  = 0x0000;
    null.base_mid  = 0x00;
    null.access    = 0x00;
    null.gran      = 0x00;
    null.base_high = 0x00;
    struct gdt_entry kc;
    kc.limit_low   = 0xFFFF;
    kc.base_low    = 0x0000;
    kc.base_mid    = 0x00;
    kc.access      = 0x9A;
    kc.gran        = 0xCF;
    kc.base_high   = 0x00;
    struct gdt_entry kd;
    kd.limit_low   = 0xFFFF;
    kd.base_low    = 0x0000;
    kd.base_mid    = 0x00;
    kd.access      = 0x92;
    kd.gran        = 0xCF;
    kd.base_high   = 0x00;
    struct gdt_entry uc;
    uc.limit_low   = 0xFFFF;
    uc.base_low    = 0x0000;
    uc.base_mid    = 0x00;
    uc.access      = 0xFA;
    uc.gran        = 0xCF;
    uc.base_high   = 0x00;
    struct gdt_entry ud;
    ud.limit_low   = 0xFFFF;
    ud.base_low    = 0x0000;
    ud.base_mid    = 0x00;
    ud.access      = 0xF2;
    ud.gran        = 0xCF;
    ud.base_high   = 0x00;
    gdt[0] = null;
    gdt[1] = kc;
    gdt[2] = kd;
    gdt[3] = uc;
    gdt[4] = ud;
    __wtss(&gdt[5]);
    static struct gdt_ptr ptr;
    ptr.limit = sizeof(gdt) - 1;
    ptr.base = (_uint32)&gdt;
    asm volatile(
        "lgdt %0" : : "m"(ptr)
    );
    // gdt reset
    asm volatile (
        "ljmp $0x08, $1f\n"
        "1:\n"
        "movw $0x10, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "movw %%ax, %%ss\n"
        : : : "eax", "memory"
    );
    flush_tss();
}
