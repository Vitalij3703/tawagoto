#include "driver/timer/timer.h"
#include "tglib/tgtype.h"

volatile _uint32* msms;

void tinit(volatile _uint32* v) {
    msms = v;
}

void tsleep(_uint ms) {
    volatile _uint32* msmss = msms;
    _uint pms = *msmss;
    while(*msms < (pms + ms))
        asm volatile ("hlt");
}