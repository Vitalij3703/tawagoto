#include "driver/key/tgkkey.h"
#include "tgkidt.h"
//#include "tgkout.h"
#include "tty/tty.h"
#include "tglib/tgtype.h"
#include "tglib/tgtconv.h"
#include "tgkernel.h"

struct rowcol {
    _uint8 scan;
    char key;
    _uint8 rowcol;
    int release;
} rowcol;

struct shift {
    char in;
    char out;
} shift;

#define NOKEY {0, '\0', 0, 0}
#define RELEASE_OFFSET 0x80

static const struct shift shiftmap[256] = {
    {'`', '~'},
    {'1', '!'},
    {'2', '@'},
    {'3', '#'},
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
    {'-', '_'},
    {'=', '+'},
    {'[', '{'},
    {']', '}'},
    {'\\', '|'},
    {';', ':'},
    {'\'', '"'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    {'a', 'A'},
    {'b', 'B'},
    {'c', 'C'},
    {'d', 'D'},
    {'e', 'E'},
    {'f', 'F'},
    {'g', 'G'},
    {'h', 'H'},
    {'i', 'I'},
    {'j', 'J'},
    {'k', 'K'},
    {'l', 'L'},
    {'m', 'M'},
    {'n', 'N'},
    {'o', 'O'},
    {'p', 'P'},
    {'q', 'Q'},
    {'r', 'R'},
    {'s', 'S'},
    {'t', 'T'},
    {'u', 'U'},
    {'v', 'V'},
    {'w', 'W'},
    {'x', 'X'},
    {'y', 'Y'},
    {'z', 'Z'},
    // extras
    {' ', ' '},
    {8, 8},
    {'\n', '\n'}
};

static const struct rowcol keys[256] = {
    NOKEY,
    {0x01, '\1', mkkey(0, 0), 0},
    {0x02, '1', mkkey(0, 1), 0},
    {0x03, '2', mkkey(0, 2), 0},
    {0x04, '3', mkkey(0, 3), 0},
    {0x05, '4', mkkey(0, 4), 0},
    {0x06, '5', mkkey(0, 5), 0},
    {0x07, '6', mkkey(0, 6), 0},
    {0x08, '7', mkkey(0, 7), 0},
    {0x09, '8', mkkey(0, 8), 0},
    {0x0A, '9', mkkey(0, 9), 0},
    {0x0B, '0', mkkey(0, 10), 0},
    {0x0C, '-', mkkey(0, 11), 0},
    {0x0D, '=', mkkey(0, 12), 0},
    {0x0E, 8, mkkey(0, 13), 0},
    {0x0F, '\t', mkkey(1, 0), 0},
    {0x10, 'q', mkkey(1, 1), 0},
    {0x11, 'w', mkkey(1, 2), 0},
    {0x12, 'e', mkkey(1, 3), 0},
    {0x13, 'r', mkkey(1, 4), 0},
    {0x14, 't', mkkey(1, 5), 0},
    {0x15, 'y', mkkey(1, 6), 0},
    {0x16, 'u', mkkey(1, 7), 0},
    {0x17, 'i', mkkey(1, 8), 0},
    {0x18, 'o', mkkey(1, 9), 0},
    {0x19, 'p', mkkey(1, 10), 0},
    {0x1A, '[', mkkey(1, 20), 0},
    {0x1B, ']', mkkey(1, 21), 0},
    {0x1C, '\n', mkkey(2, 11), 0},
    {0x1D, 3, mkkey(4, 0), 0},
    {0x1E, 'a', mkkey(2, 0), 0},
    {0x1F, 's', mkkey(2, 1), 0},
    {0x20, 'd', mkkey(2, 2), 0},
    {0x21, 'f', mkkey(2, 3), 0},
    {0x22, 'g', mkkey(2, 4), 0},
    {0x23, 'h', mkkey(2, 5), 0},
    {0x24, 'j', mkkey(2, 6), 0},
    {0x25, 'k', mkkey(2, 7), 0},
    {0x26, 'l', mkkey(2, 8), 0},
    {0x27, ';', mkkey(2, 9), 0},
    {0x28, '\'', mkkey(2, 10), 0},
    {0x29, '`', mkkey(0, 14), 0},
    {0x2A, 4, mkkey(3, 0), 0},
    {0x2B, '\\', mkkey(1, 22), 0},
    {0x2C, 'z', mkkey(3, 1), 0},
    {0x2D, 'x', mkkey(3, 2), 0},
    {0x2E, 'c', mkkey(3, 3), 0},
    {0x2F, 'v', mkkey(3, 4), 0},
    {0x30, 'b', mkkey(3, 5), 0},
    {0x31, 'n', mkkey(3, 6), 0},
    {0x32, 'm', mkkey(3, 7), 0},
    {0x33, ',', mkkey(3, 8), 0},
    {0x34, '.', mkkey(3, 9), 0},
    {0x35, '/', mkkey(3, 10), 0},
    {0x36, 4, mkkey(3, 11), 0},
    {0x37, '*', mkkey(0, 15), 0},
    NOKEY,
    {0x39, ' ', mkkey(4, 4), 0},
    {0x3A, 4, mkkey(2, 12), 0},
    NOKEY, // f1
    NOKEY, // f2
    NOKEY, // f3
    NOKEY, // f4
    NOKEY, //f5
    NOKEY, //f6
    NOKEY, //f7
    NOKEY, //f8
    NOKEY, // f9
    NOKEY, // f10
    NOKEY, // numlock
    NOKEY, // scrolllock
    NOKEY, // keypad 7
    NOKEY, // keypad 8
    NOKEY, // keypad 9
    {0x4A, '-', mkkey(0, 15), 0},
    NOKEY, // keypad 4
    NOKEY, //keypad 5
    NOKEY, // keypad 6
    {0x4E, '+', mkkey(0, 16), 0},
    NOKEY, // keypad 1
    NOKEY, // keypad 2
    NOKEY, // keypad 3
    NOKEY // keypad dot 
};

char make_upper(char c) {
    int i = 0;
    while(shiftmap[++i].in != c);
    return shiftmap[i].out;
}

_uint8 sd;

char _tgkkcstk(_uint8 scan) {
    if(scan > RELEASE_OFFSET) {
        if(keys[scan - RELEASE_OFFSET].key == '\4')
            sd = 0;
        return 0;
    }
    char kkey = keys[scan].key;
    if(kkey == '\4') {
        sd = 1;
        return 0;
    }
    return sd ? make_upper(kkey) : kkey;
}

void _tgkkpk(int inum) {
    if(inum != 33) { // just in case
        ttyputchars(":- key received wrong interrupt\n");
        return;
    }
    if(inb(0x64) & 0x01) {
        _uint8 scan = inb(0x60);
        char c = _tgkkcstk(scan);
        if(c!=0) ttyputchar(c);
    }
}

void _tgkekd() { 
    // look up ps2 commands
    ttyputchars(":- enabling ps2 keyboard driver (tgkkey init)\n");
    
    _uint8 cfg;
    while (inb(0x64) & 0x01)
        inb(0x60);
    while(inb(0x64) & 0x02);
    outb(0x64, 0x20); // read config
    while (!(inb(0x64) & 0x01));
    cfg = inb(0x60);
    cfg |= 0x40;
    while(inb(0x64) & 0x02);
    outb(0x64, 0x60); // write config
    while(inb(0x64) & 0x02);
    outb(0x60, cfg);
    while(inb(0x64) & 0x02);
    outb(0x60, 0xF4); // enable keyboard scan codes
    while(inb(0x64) & 0x01)
        inb(0x60);
    ttyputchars(":- enabled ps2 keyboard driver (tgkkey init end)\n");

}

char kkgetchar() {
    for (;;) {
        while (!(inb(0x64) & 0x01))
            asm volatile("pause");
        char c = _tgkkcstk(inb(0x60));
        if (c) return c;
    }
}

void kkreadline(char* buf) {
    char ckeyc = ' ';
    _uint ckeyi = 0;
    while(ckeyc != '\n') {
        ckeyc = kkgetchar();
        if (ckeyc == '\b' || ckeyc == 127) {
            if (ckeyi > 0) {
                ckeyi--;
                ttyputchar('\b');
                ttyputchar(' ');
                ttyputchar('\b');
            }
            continue;
        }
        if (ckeyc < 32 || ckeyc > 126) continue;
        buf[ckeyi++] = ckeyc;
        ttyputchar(ckeyc);
    }
    buf[ckeyi] = '\0';
}

void kkreadlinen(char* buf, _uint n) {
    char ckeyc;
    _uint ckeyi = 0;
    while(ckeyi != n) {
        ckeyc = kkgetchar();
        buf[ckeyi] = ckeyc;
        ttyputchar(ckeyc);
        ckeyi++;
    }
    buf[ckeyi] = '\n';
    buf[++ckeyi] = '\0';
}
