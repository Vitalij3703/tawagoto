#include "tglib/tgtconv.h"
#include "tglib/tgstr.h"
#include "mem/tgkheap.h"

#define cti(c) ((int)c - '0')

int sti(char* s) {
    int result = 0;
    int neg = s[0] == '-';
    unsigned int i = neg ? 1 : 0;
    for (; i < strlen(s); i++) {
        result = result * 10 + cti(s[i]);
    }
    return neg ? -result : result;
}

void its(int i, char* str) {
    unsigned int u = i < 0 ? 0u - (unsigned int)i : (unsigned int)i;
    int n = 0;
    if (i < 0) str[n++] = '-';
    int start = n;
    do {
        str[n++] = '0' + (u % 10);
        u /= 10;
    } while (u != 0);
    str[n] = '\0';
    for (int a = start, b = n - 1; a < b; a++, b--) {
        char t = str[a];
        str[a] = str[b];
        str[b] = t;
    }
}

char* aits(int i) {
    char* buf = kmalloc(13);
    its(i, buf);
    return buf;
}