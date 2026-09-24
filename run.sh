#!/bin/sh

qemu-system-i386 -m 512M -monitor stdio -cdrom build/tg.iso  -display sdl -enable-kvm -d int,cpu_reset -D qemu.log -no-reboot -no-shutdown -s #-S