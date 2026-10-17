#ifndef _TGGDT
#define _TGGDT
#include "tglib/tgtype.h"

extern _uint8 stack_top[];
extern _uint8 stack_bottom[];

struct gdt_entry {
    _uint16 limit_low;
    _uint16 base_low;
    _uint8 base_mid;
    _uint8 access;
    _uint8 gran; // granularity
    _uint8 base_high;
} __attribute__((packed));

struct gdt_ptr {
    _uint16 limit;
    _uint32 base;
} __attribute__((packed));

void _tgkinitgdt();

void __sks(_uint32 s);

#endif