#ifndef _TGMEM
#define _TGMEM
#include "tglib/tgtype.h"

void memcpy(void* dest, void* src, _uint b); // copy b bytes src to dest
void memmve(void* dest, char* src, _uint b); // move b bytes src to dest
int memcmp(void* a, void* b); // compare 2 ?: a, b

#endif