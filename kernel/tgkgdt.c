#include "tgkgdt.h"
#include "tglib/tgtype.h"
#include "tglib/tgstr.h"

#define GDTE 6

extern _uint32 stack_top;
extern void flush_tss(void);

struct tss_entry {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
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
    memset((_uint8*)&_tsse, 0, sizeof(_tsse));
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
