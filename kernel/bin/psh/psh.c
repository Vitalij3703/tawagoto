#include "bin/psh/psh.h"
#include "driver/key/tgkkey.h"
#include "driver/timer/timer.h"
#include "tglib/tgtype.h"
#include "tglib/tgstr.h"
#include "tglib/tgtconv.h"
//#include "tgkout.h"
#include "tty/tty.h"
#include "tgkernel.h"
#include "mem/tgkmem.h"
#include "mem/tgkvmem.h"

// commands:
//  echo [args] : output args 
//  test : output a test message
//  help : print this message

#define HMSG "commands:\n\techo [args] : output args\n\ttest : output a test message\n\ttest_wait : test the timer driver\n\ttest_bit : test physical mem alloc\n\ttest_virtual : test virtual memory mappings\n\tclear : clear the screen\n\thelp : output this message"

void exec(char* cmd) {
    if(*cmd == 0)
        return;
    int argc = 0;
    char* args[32];
    char* p = cmd;
    while(*p) {
        while(*p == ' ' || *p == '\t')
            p++;
        if(!*p) break;
        if(argc >= 31) break;
        args[argc++] = p;
        while(*p && *p != ' ' && *p != '\t')
            p++;
        if(*p) *p++ = '\0';
    }
    args[argc] = 0;
    // command handling
    ttyputchar('\n');
    if(strcmp("echo", args[0]) == 0) {
        int aargc = 1;
        while(aargc != argc) {
            ttyputchars(args[aargc++]);
            ttyputchar(' ');
        }
        ttyputchar('\n');
        return;
    }
    else if(strcmp("test", args[0]) == 0) {
        ttyputchars("This is a test message. hewwo wowd\n");
        return;
    }
    else if(strcmp("help", args[0]) == 0) {
        ttyputchars(HMSG);
        ttyputchar('\n');
        return;
    }
    else if(strcmp("panic", args[0]) == 0) {
        char** p = args+1;
        if(!*p) _tgkpanic("< No reason provided >\n\n\t(User-PSh-induced panic)");
        int tlen = 0;
        for (int i = 0; i < argc-1; i++) {
            tlen += strlen(p[i]) + 1; 
        }
        char result[256];
        for (int i = 0; i < argc-1; i++) {
            strcat(result, p[i]);
            if (i < argc - 2) {
                strcat(result, " ");
            }
        }
        _tgkpanic(format("%\n\n\t(User-PSh-induced panic)", result));
        return;
    }
    else if(strcmp("test_wait", args[0]) == 0) {
        ttyputchars("testing tsleep()\n");
        tsleep(S_TO_MS(2));
        ttyputchars("done\n");
        return;
    }
    else if(strcmp("clear", args[0]) == 0) {
        fbclear();
        return;
    }
    else if(strcmp("test_bit", args[0]) == 0) {
        ttyputchars("testing bitmap\n");
        _uint32 pages[100];
        tgkallocpages(100, pages);
        for (int i = 0; i < 100; i++) {
            char buf[13];
            its(pages[i], buf);
            ttyputcharsf("% ", buf);
        }
        tgkfreepages(pages[0], 100);
        ttyputchars("\ndone\n");
        return;
    }
    else if(strcmp("test_virtual", args[0]) == 0) {
        ttyputchars("testing virtual memory\n");
        char* test = "success\n\0";
        map_page((_uint32)test, 0x800000, 0x003);
        ttyputchars((char*)0x800000);
        ttyputchars("\ndone\n");
        return;
    }
    else {
        ttyputcharsf("Invalid command '%'! Do 'help'\n", args[0]);
        return;
    }
    
}

void __psh(struct sys_info info) {
    ttyputcharsf("\nos: %\nshell: primitive shell (PSh)\nmem: %MiB\n\n", info.os, info.mem);
    while(1) {
        char buf[256];
        ttyputchars("PSh ::> ");
        kkreadline(buf);
        exec(buf);
    }
}