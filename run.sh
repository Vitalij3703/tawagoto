#!/bin/sh

qemu-system-i386 -m 2048M -cdrom build/tg.iso  #-display sdl -enable-kvm -d int,cpu_reset -D qemu.log -no-reboot -no-shutdown -s -device bochs-display -vga qxl
