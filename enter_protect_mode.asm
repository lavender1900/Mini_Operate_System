; After POST, CPU run into real mode (16 bit)
; Real mode is the environment under which MSDOS run
; 80x86 is 32bit CPU, introduced protect mode which provide
; stronger resource protection, paging, more address space etc

org	07c00h
%include	"macros.inc"

jmp	LABEL_BEGIN

[section .gdt]
; Global Descriptor Table
			;    Base Address     Segment Limit      Attributes
DUMMY:		Descriptor		0,		0,		0 	; First descriptor is not used
DESC_CODE32:	Descriptor      SEG_CODE32,	 0ffffh,   		SA_32BIT_EXE_ONLY_CODE; Code Segment, 32bit
DESC_VIDEO:	Descriptor	0B8000h,  	   0ffffh,      SA_READ_WRITE_DATA; Video Segment, display text

; GDT end

GDTLen 		equ	$ - DUMMY
GDTPtr		dw	GDTLen - 1
		dd	DUMMY	

; Selectors
SelectorCode32	equ	DESC_CODE32 - DUMMY
SelectorVideo	equ	DESC_VIDEO - DUMMY

[section s16]
[bits 16]
LABEL_BEGIN:
; init stack
mov 	sp, 0100h

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

[section s32]
[bits 32]
SEG_CODE32:
mov	ax, SelectorVideo
mov	gs, ax

mov	edi, (80 * 11 + 79) * 2
mov	ah, 0Ch
mov	al, 'P'
mov	[gs:edi], ax

jmp	$
