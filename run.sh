#!/bin/sh

qemu-system-i386 -m 512M -cdrom build/tg.iso -vga std -display sdl -enable-kvm -d int,cpu_reset -D qemu.log -no-reboot -no-shutdown -s #-S