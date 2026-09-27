// tawagoto standard string
#include "tglib/tgstr.h"
#include "tglib/tgtype.h"

// very simple cuz it's too early to optimize

_size_t strlen(char* str) {
    for (_size_t i = 0;; i++){
        if(str[i] == '\0')
            return i;
    }    
}

int strcmp(char* a, char* b) {
    while(*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(_uint8*)a - *(_uint8*)b;
}

void strcpy(char* dest, char* src) {
    while(*dest) {
        *dest = *src;
        dest++;
        src++;
    }
}

void strncpy(char* dest, char* src, _uint n) {
    _uint i = 0;
    while(i < n) {
        *dest = *src;
        i++;
    }
}

void strcat(char* dest, char* src) { // "meow"
    char* p = dest;
    while(*p)
        p++;
    while(*src) {
        *p = *src;
        p++;
        src++;
    }
    *p = '\0';
}

int strchr(char* s, char c) {
    _uint i = 0;
    while(*s) {
        if(*s == c)
            return i;
        i++;
        s++;
    }
    return -1;
}

// if this function doesnt work, ask past me (08/09/26, alsmots 9pm) what the fuck it's supposed to do
int strrchr(char* s, char c) {
    _uint i = 0;
    while(*s) {
        s++;
        i++;
    }
    while(i) {
        if (*s == c)
            return i;
        s--;
        i--;
    }
    return -1;
}

void strrvr(char* s) {
    if (s == (void*)0 || *s == '\0') 
        return;
    int start = 0;
    int end = strlen(s) - 1;
    char temp;
    while (start < end) {
        temp = s[start];
        s[start] = s[end];
        s[end] = temp;
        start++;
        end--;
    }
}

void memset(_uint8* buf, _uint8 byte, _uint bytes) {
    for (_uint i = 0; i < bytes; i++)
        buf[i] = byte;
}