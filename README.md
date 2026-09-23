## Tawagoto
Tawagoto is a small OS, made for old x86_32 machines. Specifically, laptops.
Tawagoto was made using no AI, excepts for really repetitive stuff (the shiftmap in the PS/2 keyboard driver).
Tawagoto uses GRUB, which you must have to compile it.
There are no future plans bringing Tawagoto to other architectures.

# Install
To install/compile Tawagoto, you must first execute 'compile.sh'.
The resulting ISO, ELF, OBJ files will be in 'build/'
If you don't know which file is the ELF one, it is 'tg.bin'.

Compiling it also executes 'run.sh', which emulates the OS in QEMU.