#ifndef __PSH
#define __PSH

struct sys_info {
    char* mem;
    char* os;
};

void __psh(struct sys_info info);

#endif