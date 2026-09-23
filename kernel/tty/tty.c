#include "tty/tty.h"
#include "boot/multiboot.h"
#include "tglib/tgtype.h"
#include "tgkernel.h"
#include "tty/font8x8/font8x8_basic.h"
#include "tglib/tgstr.h"
#include <stdarg.h>

_uint8* fb;
_uint32  fbend;
_uint ccrow; // y
_uint cccol; // x
_uint height;
_uint width;
_uint stride;
_uint max_char_col;
_uint max_char_row;
_uint b8pp;
_uint maxpx; // basically framebuffer len

_uint32 bg_color;
_uint32 fg_color;

void fbinit(multiboot_info_t* info) {
    if((info->flags & (1 << 12)) == 0)
        _tgkpanic("Framebuffer unable to initialize");
    fb = (_uint8*)info->framebuffer_addr;
    height = info->framebuffer_height;
    width = info->framebuffer_width;
    stride = info->framebuffer_pitch;
    b8pp = info->framebuffer_bpp / 8;
    maxpx= width*height;
    fbend = (_uint32)fb + maxpx;
    ccrow = 0;
    cccol = 0;
    max_char_col = width/8;
    max_char_row = height/8;
    bg_color = 0;
    fg_color = 0xFFFFFF;
    //for (int i = 0; i < height; i++) // for testing
    //    for (int ii = 0; ii < width; ii++)
    //        fbdrawpx(ii, i, DEFAULT_FG_COL);
    fbfill(bg_color);
    //ttyputchars(" tawagoto sucks. w nights spider");
}

_uint32 gbg() {
    return bg_color;
}
_uint32 gfg() {
    return bg_color;
}

void fbdrawpx(_uint x, _uint y, _uint32 color) {
    _uint8 *ptr = fb + y * stride + x * b8pp;

    for (_uint i = 0; i < b8pp; i++) {
        ptr[i] = (color >> (i * 8)) & 0xFF;
    }
}

void fbfill(_uint32 color) {
    for (int i = 0; i < height; i++)
        for (int ii = 0; ii < width; ii++)
            fbdrawpx(ii, i, color);
}

void fbclear() {
    fbfill(bg_color);
    cccol = 0;
    ccrow = 0;
}

void ttydrawchar(char c, _uint x, _uint y) {
    for (int i = 0; i < 8; i++) {
        char charrow = font8x8_basic[c][i];
        for (int ii = 0; ii < 8; ii++) {
            if(((charrow >> ii)&1) != 0) {
                fbdrawpx(x+ii, y+i, fg_color);
            } else {
                fbdrawpx(x+ii, y+i, bg_color);
            }
        }
    }
    
}

void ttyputchar(char c) {
    if(c == '\n') {
        ccrow++;
        cccol = 0;
        return;
    }
    if(cccol >= stride) {
        ccrow++;
        cccol=0;
    }
    if(c == 8) {
        cccol--;
        return;
    }
    ttydrawchar(c, cccol++*8, ccrow*8);
}

void ttyputchars(char* cs) {
    char* p = cs;
    while(*p != '\0') {
        ttyputchar(*p);
        p++;
    }
}

void ttychangecolor(_uint32 colorbg, _uint32 colorfg) {
    bg_color = colorbg;
    fg_color = colorfg;
}

// yes i copy pasted from tgkout
void ttyputcharsf(char* str, ...) {
    va_list args;
    va_start(args, str);
    for (_size32 i = 0; i < strlen(str); i++) {
        if(str[i] == '%') {
            if(str[i+1] == '%') {
                ttyputchar('%');
                continue;
            }
            ttyputchars(va_arg(args, char*));
            continue;
        }
        ttyputchar(str[i]);
    }
    va_end(args);
}

char* format(char* str, ...) {
    va_list args;
    va_start(args, str);
    int index = 0;
    static char stri[1024];
    for (_size32 i = 0; i < strlen(str); i++) {
        if(str[i] == '%') {
            if(str[i+1] == '%') {
                stri[index] = '%';
                continue;
            }
            char* s = va_arg(args, char*);
            while(*s)
                stri[index++] = *s++;
            continue;
        }
        stri[index] = str[i];
        index++;
    }
    va_end(args);
    return stri;
}
