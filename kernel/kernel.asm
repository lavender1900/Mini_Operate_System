SELECTOR_KERNEL_CS	equ	8

extern	cstart
extern	gdt_ptr

[section .text]
align 32
[bits 32]
global	_start

_start:
mov	esp, StackTop
sgdt	[gdt_ptr]
call	cstart
lgdt	[gdt_ptr]

jmp	SELECTOR_KERNEL_CS:csinit

csinit:
push	0
popfd

hlt

[section .bss]
StackSpace:	resb	2 * 1024
StackTop:
