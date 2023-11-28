#!/bin/bash
set -e


echo "[Build] Compiling ASM"

nasm -f elf32 src/boot.asm -o build/boot.o
nasm -f elf32 src/asm/irq.asm -o build/irq.o
nasm -f elf32 src/asm/load_idt.asm -o build/load_idt.o
nasm -f elf32 src/asm/load_gdt.asm -o build/load_gdt.o
nasm -f elf32 src/asm/bios32_call.asm -o build/bios32_call.o
nasm -f elf32 src/asm/exception.asm -o build/exception.o

echo "[Build] Compiling Kernel"
clang -c -target i686-none-elf -o kernel.o -ffreestanding -mno-sse -Wall src/*.c -o kernel.o -Isrc/include -fno-stack-protector -U_FORTIFY_SOURCE

echo "[Build] Linking"
ld.lld -T linker.ld -o os.bin -static -nostdlib build/boot.o build/load_idt.o build/load_gdt.o build/irq.o build/exception.o build/bios32_call.o kernel.o


echo "[Build] Clearing Old Build..."
rm build/*.o
rm *.o

echo "[Build] Building ISO"
cp os.bin iso/boot/os.bin
grub-mkrescue -o os.iso iso

echo "[Build] Booting i386 (-cdrom)"
qemu-system-i386 -cdrom os.iso -serial file:serial.log # -d guest_errors,int