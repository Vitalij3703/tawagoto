// in loving memory of tgkout
// vga text buffer misses you
/*
#include "tgkout.h"
#include "tgkernel.h"
#include "tglib/tgtype.h"
#include "tglib/tgstr.h"
#include "tty/tty.h"
#include <stdarg.h>

#define _GI(x, y) (y * __vgaw + x) 

#define _TAB_LENGHT 4

volatile char* _tgkscreen = (volatile char*)0xB8000;

_size32 crow; // y
_size32 ccol; // x
_uint8 col;
volatile char* cbuf;

__attribute_deprecated__
void tgkcinit(void) {
    crow = 0;
    ccol = 0;
    cbuf = _tgkscreen;
    col = 0x0f;
}

__attribute_deprecated__
void tgkcsc(_uint32 bg, _uint32 fg) {
    ttychangecolor(bg, fg);
}

//int last = 0;

__attribute_deprecated__
void tgkprintc(char c) {
    unsigned char uc = c;
    if(c == '\n') {
        crow++;
        ccol = 0;
        if (crow == __vgah) {
		    tgkss();
		    tgkdll();
		    crow = __vgah-1;
	    }
        return;
    }
    if(c == '\t') {
        ccol += _TAB_LENGHT;
        return;
    }
    if(c == 8) {
        if(ccol == 0) {
            if(crow == 0) return;
            crow--;
            ccol = __vgaw;
            while(cbuf[(_GI(ccol, crow)-1) * 2] < 33)
                ccol--;
        }
        tgkpca(' ', --ccol, crow);
        return;
    }
    if(c < 32)
        return;
    tgkpca(uc, ccol, crow);
    tgksetcursor(ccol+1, crow);
	if (++ccol == __vgaw || crow+2 == __vgah) {
		ccol = 0;
        if (++crow == __vgah) {
	    	tgkss();
	    	tgkdll();
	    }
    }
}

__attribute_deprecated__
void tgkprint(char* str) {
    for (_size32 i = 0; i < strlen(str); i++) {
        tgkprintc(str[i]);
    }
}

__attribute_deprecated__
void tgkprintf(char* str, ...) {
    va_list args;
    va_start(args, str);
    for (_size32 i = 0; i < strlen(str); i++) {
        if(str[i] == '%') {
            if(str[i+1] == '%') {
                tgkprintc('%');
                continue;
            }
            tgkprint(va_arg(args, char*));
            continue;
        }
        tgkprintc(str[i]);
    }
    va_end(args);
}

__attribute_deprecated__
void tgkprintac(char c) {
    char cc[2] = {c, '\0'};
    tgkprint(cc);
}

__attribute_deprecated__
char* tgkformat(char* str, ...) {
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

// straight from osdev org
// deprecated anyways
__attribute_deprecated__
void tgksetcursor(_uint x, _uint y) {
	uint16_t pos = y * __vgaw + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

*/