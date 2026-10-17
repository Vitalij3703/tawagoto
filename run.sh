#!/bin/sh

qemu-system-x86_64 -cdrom build/tg.iso -debugcon file:qemu.log -no-reboot -s

#qemu-system-x86_64 -machine q35   -drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd   -drive if=pflash,format=raw,file=vars.fd   -cdrom build/tg.iso -debugcon file:qemu.log -no-reboot -s