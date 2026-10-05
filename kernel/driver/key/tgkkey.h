#ifndef _TGKKEY
#define _TGKKEY
#include "tglib/tgtype.h"

// the keyboard driver

#define key _uint8
#define mkkey(row, col) (((col&0x1F) << 3) | (row&0x07))
#define rfkey(k) (k&0x07)
#define cfkey(k) (k>>3);

void _tgkekd(); // enable keyboard driver
void _tgkkpk(int inum); // for debugging only
char _tgkkcstk(_uint8 scan); // convert scan to key 
char kkgetchar(); // get a single character from the keyboard
void kkreadline(char* buf); // write to buf until \\n
void kkreadlinen(char* buf, _uint n); // write to buf until n, make sure buf has the size of n+1

#endif