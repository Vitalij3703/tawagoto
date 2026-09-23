#ifndef __TGBMB
#define __TGBMB
#include "tglib/tgtype.h"
typedef struct {
    _uint32 flags;
    _uint32 mem_lower;
    _uint32 mem_upper;
    _uint32 boot_device;
    _uint32 cmdline;
    _uint32 mods_count;
    _uint32 mods_addr;
    _uint32 syms[4];
    _uint32 mmap_length;
    _uint32 mmap_addr;
    _uint32 drives_length;
    _uint32 drives_addr;
    _uint32 config_table;
    _uint32 boot_loader_name;
    _uint32 apm_table;
    _uint32 vbe_control_info;
    _uint32 vbe_mode_info;
    _uint16 vbe_mode;
    _uint16 vbe_interface_seg;
    _uint16 vbe_interface_off;
    _uint16 vbe_interface_len;
    _uint64 framebuffer_addr;
    _uint32 framebuffer_pitch;
    _uint32 framebuffer_width;
    _uint32 framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint8_t  color_info[6];
} __attribute__((packed)) multiboot_info_t;
struct mmap_entry {
    _uint32 size;
    _uint64 baddr; // base address
    _uint64 len;
    _uint32 type;
} __attribute__((packed));
#endif