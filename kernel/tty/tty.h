#ifndef _TGTTY
#define _TGTTY
#include "boot/multiboot.h"
#include "tglib/tgtype.h"

// credits to Daniel Hepper for the 8x8 font
// github.com/dhepper/font8x8

// may be changed in the future
#define RGB_TO_COLOR(r, g, b) \
((r<<16) | (g<<8) | (b)) 

#define TAWAGOTO_VERSION 0

void fbinit(multiboot_info_t* info);

void fbdrawpx(_uint x, _uint y, _uint32 color); // use RGB_TO_COLOR for color

void fbfill(_uint32 color);

void fbclear();

void ttydrawchar(char c, _uint x, _uint y);

void ttyputchar(char c);

void ttyputchars(char* cs);

void ttychangecolor(_uint32 colorbg, _uint32 colorfg);

void ttyputcharsf(char* str, ...);

char* format(char* str, ...);

_uint32 gbg();
_uint32 gfg();

#endif