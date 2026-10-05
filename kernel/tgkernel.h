// tawagoto kernel
#ifndef __TGK
#define __TGK
#define _ATR_DEFAULT 0x07
#include "tglib/tgtype.h"
#include "boot/multiboot.h"

void _tgkmain(_size32 magic, multiboot_info_t* mbi); // the kernel main
void _tgkpanic(char *reason); // kernel panic
void __tgkhang__(void);
_uint8 inb(_uint16 p);
void outb(_uint16 p, _uint8 v);
void enter_usr(void);
void user_continue(void);

#endif