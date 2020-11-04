; After POST, CPU run into real mode (16 bit)
; Real mode is the environment under which MSDOS run
; 80x86 is 32bit CPU, introduced protect mode which provide
; stronger resource protection, paging, more address space etc

; For me there's two reason running these codes under freedos
; 1. As code grows, it will exceed 512 bytes very soon
; 2. NASM's support for referring labels between different sections is very poor,
;    which means I can't write bootable code like '510 - ($ - START) db 0'
;
; But don't worry, I will write my own loader in the future

%include	"macros.inc"

org	0100h ; nasm directive - relocate label address by 0x0100

jmp	BEGIN	

[section .gdt]
; Global Descriptor Table
GDT_START:
			;    Base Address     Segment Limit      Attributes
GDT_DUMMY:	Descriptor	0,		0,			0 			; First descriptor is not used
DESC_CODE32:	Descriptor     	0,		0ffffh,   		DA_32BIT_EXE_ONLY_CODE
DESC_C32_RING1:	Descriptor	0,		0fffh,			DA_32BIT_EXE_ONLY_CODE | DA_DPL1
DESC_CODE16:	Descriptor	0,		0ffffh,			DA_16BIT_EXE_ONLY_CODE
DESC_STACK:	Descriptor	0,		TopOfStack,		DA_32BIT_READ_WRITE_STACK
DESC_STACK1	Descriptor	0,		TopOfStack1,		DA_32BIT_READ_WRITE_STACK | DA_DPL1
DESC_DATA:	Descriptor	0,		DataLen - 1,		DA_READ_WRITE_DATA | DA_DPL3
DESC_VIDEO:	Descriptor	0B8000h,  	0ffffh,     		DA_READ_WRITE_DATA | DA_DPL3
DESC_LDT:	Descriptor	0,		LDTLen - 1,		DA_LDT
DESC_HIGH_MEM:	Descriptor	0500000h,	0ffffh,			DA_READ_WRITE_DATA
DESC_CALL_GATE: Descriptor	0,		0ffffh,			DA_32BIT_EXE_ONLY_CODE 

; Return from 32bit mode, we need a proper selector loading into ds, es, ss etc
DESC_WORK_AROUND Descriptor	0,		0ffffh,			DA_READ_WRITE_DATA	

; TSS
DESC_TSS:	Descriptor	0, 		TSSLen - 1,		89h	

; Call Gate
			; Target Selector	offset	DCount		Attribute
CALL_GATE:	Gate	SelectorCallGate,	0,	0,		DA_386CALL_GATE | DA_DPL3	

; GDT end

GDTLen 		equ	$ - GDT_DUMMY
GDTPtr		dw	GDTLen - 1
		dd	0	

; Selectors
SelectorCode32		equ	DESC_CODE32 - GDT_DUMMY
SelectorCodeRing1	equ	(DESC_C32_RING1 - GDT_DUMMY) | SA_RPL1
SelectorCode16		equ	DESC_CODE16 - GDT_DUMMY
SelectorData		equ	DESC_DATA - GDT_DUMMY 
SelectorStack		equ	DESC_STACK - GDT_DUMMY
SelectorStack1		equ	(DESC_STACK1 - GDT_DUMMY) | SA_RPL1 
SelectorVideo		equ	DESC_VIDEO - GDT_DUMMY
SelectorLdt		equ	DESC_LDT - GDT_DUMMY
SelectorHighMem		equ	DESC_HIGH_MEM - GDT_DUMMY	
SelectorTss		equ	DESC_TSS - GDT_DUMMY
SelectorCallGate	equ	DESC_CALL_GATE - GDT_DUMMY
SelectorCallGateSelf	equ	CALL_GATE - GDT_DUMMY
SelectorWorkAround	equ	DESC_WORK_AROUND - GDT_DUMMY

[section .s16]
align 16
[bits 16]
BEGIN:
; init stack
mov	ax, cs
mov	ds, ax
mov	es, ax
mov	ss, ax
mov 	sp, 0100h

; prepare for jump back real mode
mov	[GO_BACK_TO_REAL_MODE + 3], ax
mov	word	[SPValueInRealMode], sp

; init GDT loading info
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, GDT_START 
mov	dword	[GDTPtr + 2], eax

; init LDT Descriptor in GDT
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, LDT_START 
mov	word	[DESC_LDT + 2], ax
shr	eax, 16
mov	byte	[DESC_LDT + 4], al
mov	byte	[DESC_LDT + 7], ah

; init Descriptor in LDT
xor	eax, eax
mov	ax, ds
shl	eax, 4
add	eax, SEG_CODE_LDT 
mov	word	[LDT_DESC_CODE + 2], ax
shr	eax, 16
mov	byte	[LDT_DESC_CODE + 4], al
mov	byte	[LDT_DESC_CODE + 7], ah

; init TSS Segment
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, SEG_TSS
mov	word	[DESC_TSS + 2], ax
shr	eax, 16
mov	byte	[DESC_TSS + 4], al
mov	byte	[DESC_TSS + 7], ah

; init Call Gate Segment
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, SEG_CALL_GATE
mov	word	[DESC_CALL_GATE + 2], ax
shr	eax, 16
mov	byte	[DESC_CALL_GATE + 4], al
mov	byte	[DESC_CALL_GATE + 7], ah

; init Ring3 32bit Code Segment
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, SEG_CODE_RING1
mov	word	[DESC_C32_RING1 + 2], ax
shr	eax, 16
mov	byte	[DESC_C32_RING1 + 4], al
mov	byte	[DESC_C32_RING1 + 7], ah

; init 32bit Code Segment Base Address
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, SEG_CODE32
mov	word	[DESC_CODE32 + 2], ax
shr	eax, 16
mov	byte	[DESC_CODE32 + 4], al
mov	byte	[DESC_CODE32 + 7], ah

; init 16bit Code Segment Base Address
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, SEG_CODE16
mov	word	[DESC_CODE16 + 2], ax
shr	eax, 16
mov	byte	[DESC_CODE16 + 4], al
mov	byte	[DESC_CODE16 + 7], ah

; init Data Segment Base Address
xor	eax, eax
mov	ax, cs
shl	eax, 4
add 	eax, SEG_DATA
mov	word	[DESC_DATA + 2], ax
shr	eax, 16
mov	byte	[DESC_DATA + 4], al
mov	byte	[DESC_DATA + 7], ah

; init Stack Segment Base Address
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, SEG_STACK
mov	word	[DESC_STACK + 2], ax
shr	eax, 16
mov	byte	[DESC_STACK + 4], al
mov	byte 	[DESC_STACK + 7], ah

; init Ring3 Stack Segment
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, SEG_STACK_RING1
mov	word	[DESC_STACK1 + 2], ax
shr	eax, 16
mov	byte	[DESC_STACK1 + 4], al
mov	byte	[DESC_STACK1 + 7], ah

; load GDTR
lgdt	[GDTPtr]

cli

in	al, 92h
or	al, 10b
out	92h, al

mov	eax, cr0
or	eax, 1
mov	cr0, eax

jmp	dword SelectorCode32: 0 ; jump into protect mode

[section .tss]
align 32
[bits 32]
SEG_TSS:
dd	0
dd	TopOfStack
dd	SelectorStack
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dd	0
dw	0
dw	$ - SEG_TSS + 2
db	0ffh
TSSLen	equ	$ - SEG_TSS

[section .ldt]
align 32
[bits 32]
LDT_START:
LDT_DUMMY		Descriptor	0,	0,		0
LDT_DESC_CODE		Descriptor	0,	0ffffh,		DA_32BIT_EXE_ONLY_CODE

LDTLen		equ		$ - LDT_DUMMY

SelectorLdtCode		equ		(LDT_DESC_CODE - LDT_DUMMY) | SA_LOCAL

[section .s16protectmode]
align 16
[bits 16]
SEG_CODE16:
mov	ax, SelectorWorkAround
mov	ds, ax
mov	es, ax
mov	fs, ax
mov	gs, ax
mov	ss, ax

; Close protect mode
mov	eax, cr0
and	al, 11111110b
mov	cr0, eax

; Jump back to 16 bit real mode
GO_BACK_TO_REAL_MODE:
jmp	0: REAL_MODE

[section .s16realmode]
align 16
[bits 16]
REAL_MODE:
mov	ax, cs
mov	ds, ax
mov	es, ax
mov	ss, ax

mov	sp, [SPValueInRealMode]

in	al, 92h
and	al, 11111101b
out	92h, al

sti

; return to freedos
mov	ax, 4c00h
int	21h

[section .data]
align 32
[bits 32]
SEG_DATA:
SPValueInRealMode	dw	0
PMMessage:	db	"In Protect Mode now. ^-^",0
OffsetMessage	equ	PMMessage - $$
StrTest:	db	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",0
OffsetStrTest	equ	StrTest - $$
DataLen		equ	$ - SEG_DATA

[section .stack]
align 32
[bits 32]
SEG_STACK:
times	512	db	0
TopOfStack	equ	$ - SEG_STACK - 1

[section .stackring3]
align 32
[bits 32]
SEG_STACK_RING1:
times	512	db	0
TopOfStack1	equ	$ - SEG_STACK_RING1 - 1

[section .code32]
align 32
[bits 32]
SEG_CODE32:
; load tss
mov	ax, SelectorTss
ltr	ax

; Switch to Ring1
push	SelectorStack1
push	TopOfStack1
push	SelectorCodeRing1
push	0
retf


mov	ax, SelectorData
mov	ds, ax
mov	ax, SelectorHighMem
mov	es, ax
mov	ax, SelectorVideo
mov	gs, ax

mov	ax, SelectorStack
mov	ss, ax

mov	esp, TopOfStack

; load LDTR
mov	ax, SelectorLdt
lldt	ax

call	SelectorCallGateSelf:0

; call function through LDT
jmp	SelectorLdtCode:0


[section .code32ldt]
align 32
[bits 32]
SEG_CODE_LDT:
mov	ax, SelectorVideo
mov	gs, ax
mov	edi, (80 * 9 + 0) * 2
mov	ah, 0Ch
mov	al, 'L'
mov	[gs:edi], ax

mov	ah, 0Ch ; 00001100 -  0000 black background 1100 red chars
xor	esi, esi
xor	edi, edi
mov	esi, OffsetMessage
mov	edi, (80 * 10 + 0) * 2
cld

.1:
lodsb
test	al, al
jz	.2
mov	[gs:edi], ax
add	edi, 2
jmp	.1

.2:
call	DisplayReturn
call	ReadHighMem
call	WriteHighMem
call	ReadHighMem

jmp	SelectorCode16:0	

[section .callgate]
align	32
[bits 32]
SEG_CALL_GATE:
mov	ax, SelectorVideo
mov	gs, ax
mov	edi, (80 * 15 + 0) * 2
mov	ah, 0Ch
mov	al, 'C'
mov	[gs:edi], ax

retf

[section .codering1]
align 32
[bits 32]
SEG_CODE_RING1:
mov	ax, SelectorVideo
mov	gs, ax
mov	edi, (80 * 20 + 0) * 2
mov	ah, 0Ch
mov	al, '7'
mov	[gs:edi], ax
xchg	bx, bx
call	SelectorCallGateSelf:0

jmp	$

;--------------- ReadHighMem() start  -------------------
ReadHighMem:
xor	esi, esi
mov	ecx, 8
.loop:
mov	al, [es:esi]
call	DisplayAL
inc	esi
loop	.loop

call	DisplayReturn

ret
;-------------- ReadHighMem() end ---------------------

;-------------- WriteHighMem() start -------------------
WriteHighMem:
push	esi
push	edi
xor	esi, esi
xor	edi, edi
mov	esi, OffsetStrTest
cld

.1:
lodsb
test	al, al
jz	.2
mov	[es:edi], al
inc	edi
jmp	.1
.2:
pop	edi
pop	esi

ret
;-------------- WriteHighMem() end -------------------

;-------------- DisplayAL() start --------------------
DisplayAL:
push	ecx
push	edx

; This function convert decimal to hex
mov	ah, 0Ch
mov	dl, al
; Each time process only four bits (as four bits perform max hex F(1111))
shr	al, 4
mov	ecx, 2
.begin:
and	al, 01111b
cmp	al, 9
ja	.1
add	al, '0'
jmp	.2
.1:
sub	al, 0Ah
add	al, 'A'

.2:
mov	[gs:edi], ax
add	edi, 2
mov	al, dl
loop	.begin

add	edi, 2

pop	edx
pop	ecx

ret
;-------------- DisplayAL() end ---------------------

;-------------- DisplayReturn() start ---------------------
DisplayReturn:
push	eax
push	ebx
mov	eax, edi
mov	bl, 160
div	bl
and	eax, 0ffh
inc	eax
mov	bl, 160
mul	bl
mov	edi, eax
pop	ebx
pop	eax

ret
;-------------- DisplayReturn() end ------------------------
