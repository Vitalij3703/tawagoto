#include "tgkgdt.h"

#define GDTE 3

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
    gdt[0] = null;
    gdt[1] = kc;
    gdt[2] = kd;
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
}