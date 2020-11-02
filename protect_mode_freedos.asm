; After POST, CPU run into real mode (16 bit)
; Real mode is the environment under which MSDOS run
; 80x86 is 32bit CPU, introduced protect mode which provide
; stronger resource protection, paging, more address space etc

%include	"macros.inc"

org	0100h ; nasm directive - relocate label address by 0x0100

jmp	BEGIN	

[section .gdt]
; Global Descriptor Table
			;    Base Address     Segment Limit      Attributes
DUMMY:		Descriptor	0,		0,			0 			; First descriptor is not used
DESC_CODE32:	Descriptor     	0,		0ffffh,   		DA_32BIT_EXE_ONLY_CODE	
DESC_STACK:	Descriptor	0,		TopOfStack,		DA_32BIT_READ_WRITE_STACK
DESC_DATA:	Descriptor	0,		DataLen - 1,		DA_READ_WRITE_DATA
DESC_VIDEO:	Descriptor	0B8000h,  	0ffffh,     		DA_READ_WRITE_DATA 
DESC_HIGH_MEM:	Descriptor	0500000h,	0ffffh,			DA_READ_WRITE_DATA

; GDT end

GDTLen 		equ	$ - DUMMY
GDTPtr		dw	GDTLen - 1
		dd	0	

; Selectors
SelectorCode32		equ	DESC_CODE32 - DUMMY
SelectorData		equ	DESC_DATA - DUMMY
SelectorStack		equ	DESC_STACK - DUMMY
SelectorVideo		equ	DESC_VIDEO - DUMMY
SelectorHighMem		equ	DESC_HIGH_MEM - DUMMY	

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

; init GDT loading info
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, DUMMY
mov	dword	[GDTPtr + 2], eax

; init Code Segment Base Address
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, SEG_CODE32
mov	word	[DESC_CODE32 + 2], ax
shr	eax, 16
mov	byte	[DESC_CODE32 + 4], al
mov	byte	[DESC_CODE32 + 7], ah

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


[bits 32]
SEG_CODE32:
mov	ax, SelectorData
mov	ds, ax
mov	ax, SelectorHighMem
mov	es, ax
mov	ax, SelectorVideo
mov	gs, ax

mov	ax, SelectorStack
mov	ss, ax

mov	esp, TopOfStack

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
