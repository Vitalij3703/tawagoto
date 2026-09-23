/*#ifndef _TGCONSOLE
#define _TGCONSOLE
#include "tglib/tgtype.h"

#define TAWAGOTO_VERSION 0

#define __vgaw 80
#define __vgah 25

void tgkcinit(void); // init the console
//void tgkprintc(char c); // print a character
void tgkprintf(char* str, ...); // like tgkprint but also allows for formatting like %
void tgkprint(char* str); // output str to vga
//void tgkpca(char c, _uint x, _uint y);
void tgkcfill(char f);
void tgkcsc(_uint8 c);
void tgkprintac(char c);
char* tgkformat(char* str, ...);
void tgksetcursor(_uint x, _uint y);

#endif
*/