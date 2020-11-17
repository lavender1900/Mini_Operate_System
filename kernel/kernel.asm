SELECTOR_KERNEL_CS	equ	8
SELECTOR_TSS		equ	0x20
SELECTOR_LDT		equ	0x28

extern	cstart
extern	gdt_ptr
extern	idt_ptr
extern	exception_handler
extern	spurious_irq
extern	kernel_main
extern	proc_table
extern	PROCESS_TABLE_LDT_SELECTOR_OFFSET
extern	PROCESS_TABLE_TSS_OFFSET

global	_start
global	restart

global	StackRing3Top
global	StackRing0Top

global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error	

global	hwint00
global	hwint01
global	hwint02
global	hwint03
global	hwint04
global	hwint05
global	hwint06
global	hwint07
global	hwint08
global	hwint09
global	hwint10
global	hwint11
global	hwint12
global	hwint13
global	hwint14
global	hwint15

[section .text]
align 32
[bits 32]

_start:
mov	esp, StackRing0Top
sgdt	[gdt_ptr]
call	cstart
lgdt	[gdt_ptr]

jmp	SELECTOR_KERNEL_CS:csinit

csinit:
lidt	[idt_ptr]
sti

jmp	kernel_main

restart:
mov	esp, proc_table
xor	eax, eax
mov	ax, SELECTOR_TSS
ltr	ax
xor	eax, eax
mov	eax, [PROCESS_TABLE_LDT_SELECTOR_OFFSET]
lldt	[esp + eax]	

pop	gs
pop	fs
pop	es
pop	ds
lea	eax, [esp + 32]
mov	[esp + 12], eax
popad
add	esp, 4

iretd

;************ Intel Used Interrupts *****************
divide_error:
push	0xFFFFFFFF
push	0
jmp	exception

single_step_exception:
push	0xFFFFFFFF
push	1
jmp	exception

nmi:
push	0xFFFFFFFF
push	2
jmp	exception

breakpoint_exception:
push	0xFFFFFFFF
push	3
jmp	exception

overflow:
push	0xFFFFFFFF
push	4
jmp	exception

bounds_check:
push	0xFFFFFFFF
push	5
jmp	exception

inval_opcode:
push	0xFFFFFFFF
push	6
jmp	exception

copr_not_available:
push	0xFFFFFFFF
push	7
jmp	exception

double_fault:
push	8
jmp	exception

copr_seg_overrun:
push	0xFFFFFFFF
push	9
jmp	exception	

inval_tss:
push	10
jmp	exception

segment_not_present:
push	11
jmp	exception

stack_exception:
push	12
jmp	exception

general_protection:
push	13
jmp	exception

page_fault:
push	14
jmp	exception

copr_error:
push	0xFFFFFFFF
push	15
jmp	exception

exception:
call	exception_handler
add	esp, 8
hlt
iretd

;*************** Customized 8259A interrupts ********************
hwint00:
push	0
jmp	hexception

hwint01:
push	1
jmp	hexception

hwint02:
push	2
jmp	hexception

hwint03:
push	3
jmp	hexception

hwint04:
push	4
jmp	hexception

hwint05:
push	5
jmp	hexception

hwint06:
push	6
jmp	hexception

hwint07:
push	7
jmp	hexception

hwint08:
push	8
jmp	hexception

hwint09:
push	9
jmp	hexception

hwint10:
push	10
jmp	hexception

hwint11:
push	11
jmp	hexception

hwint12:
push	12
jmp	hexception

hwint13:
push	13
jmp	hexception

hwint14:
push	14
jmp	hexception

hwint15:
push	15
jmp	hexception

hexception:
call	spurious_irq
add	esp, 4
iretd

[section .bss]
StackRing0:	resb	2 * 1024
StackRing0Top: 

StackRing3:	resb	2 * 1024
StackRing3Top:
