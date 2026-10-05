#include "tglib/tgtconv.h"
#include "tglib/tgstr.h"

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

#define itc(i) ((char)(i + '0'))

void its(int i, char* str) {
    *str = i < 0 ? '-' : ' '; str[12] = '\0';
    int ii = i < 0 ? 1 : 0;
    int result = i;
    while(result != 0){
        str[ii] = itc(result % 10);
        result /= 10;
        ii++;
    }
    strrvr(str);
    if(i == 0) str[0] = '0';
}