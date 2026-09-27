#ifndef _TGSTRING
#define _TGSTRING
#include "tglib/tgtype.h"

_size_t strlen(char* str); // get the lenght of *str*
int strcmp(char* a, char* b); // compare 2 string: a, b
void strcpy(char* dest, char* src); // copy strings src to dest
void strncpy(char* dest, char* src, _uint n); // copy n chars from strings src to dest
void strcat(char* dest, char* src); // append src onto dest
int strchr(char* s, char c); // find first char c in string s, returns the index
int strrchr(char* s, char c); // find last char c in string s, returns the index
void strrvr(char* s); // reverse a string
void memset(_uint8* buf, _uint8 byte, _uint bytes);

#endif