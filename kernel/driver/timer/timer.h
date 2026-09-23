#ifndef __TIMERD
#define __TIMERD
#include "tglib/tgtype.h"

void tinit(volatile _uint32* v); // input a reference to the timer variable

void tsleep(_uint ms); // this functiono may be very inaccurate, it is not suitable for precision

#define MS_TO_S(ms) (ms/1000)
#define S_TO_MS(s)  (s*1000)

#endif