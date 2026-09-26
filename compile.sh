#!/bin/sh

#i know i will have to switch to make once this project gets big.

# clean up
rm -rf build
rm iso/boot/tg.bin
mkdir build
cd build
#compile
nasm -f elf32 ../kernel/boot/tgboot.asm -o tgb.o
i686-elf-gcc -c ../kernel/tglib/*.c -I ../kernel/ -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-gcc -c ../kernel/tgkernel.c -I ../kernel/ -o tgk.o -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-gcc -c ../kernel/tgkout.c -I ../kernel/ -o tgo.o -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-gcc -c ../kernel/tgkgdt.c -I ../kernel/ -o tgg.o -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-gcc -c ../kernel/tgkidt.c -I ../kernel/ -o tgi.o -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-gcc -c ../kernel/mem/tgkmem.c -I ../kernel/ -o tgm.o -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-gcc -c ../kernel/mem/tgkvmem.c -I ../kernel/ -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-gcc -c ../kernel/mem/tgkheap.c -I ../kernel/ -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-gcc -c ../kernel/tty/tty.c -I ../kernel/ -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
pwd
for f in ../kernel/driver/*/*.c; do
    pwd
    i686-elf-gcc -c "$f" -o "kd_$(basename "$f" .c).o" -I ../kernel/ -std=gnu99 -ffreestanding -O2 -Wall -Wextra
done
for f in ../kernel/bin/*/*.c; do
    pwd
    i686-elf-gcc -c "$f" -o "kb_$(basename "$f" .c).o" -I ../kernel/ -std=gnu99 -ffreestanding -O2 -Wall -Wextra
done
nasm -f elf32 ../kernel/is.asm -o tgis.o
i686-elf-gcc -T ../kernel/boot/linker.ld -o tg.bin -ffreestanding -O0 -nostdlib *.o -lgcc -g
cp tg.bin ../iso/boot/tg.bin
grub-mkrescue -o tg.iso ../iso

#vm
cd ..
./run.sh