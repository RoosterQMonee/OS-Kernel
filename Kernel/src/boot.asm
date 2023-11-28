MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
MBFLAGS  equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + MBFLAGS)

BOOTLOADER_MAGIC  equ  0x2BADB002

section .multiboot
  align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM


section .bss

align 16
stack_bottom:

resb 1048576 ; 1 MiB
stack_top:


section .text
    global _start: function (_start.end - _start)
    global MAGIC_HEADER
    global BOOTLOADER_MAGIC

_start:
	mov esp, stack_top
	extern kernel_main
  
  mov eax, BOOTLOADER_MAGIC
  push ebx
  push eax
  
	call kernel_main
	cli

.hang:	hlt
	jmp .hang

.end: