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
DESC_CODE32:	Descriptor		0, 0ffffh,   		0; Code Segment, 32bit
DESC_VIDEO:	Descriptor	0B8000h,  	   0ffffh,             	0; Video Segment, display text

; GDT end

GDTLen 		equ	$ - DUMMY
GDTPtr		dw	GDTLen - 1
		dd	0

SelectorCode32	equ	DESC_CODE32 - DUMMY
SelectorVideo	equ	DESC_VIDEO - DUMMY

LABEL_BEGIN:
mov	ax, DUMMY
xor	ebx, ebx
mov	bx, cs
shl	ebx, 4
add	ebx, DUMMY
jmp	$
