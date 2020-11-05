; Hah! Not easy to get here, pure assembly language programming is really
; hard to track relations among codes.

; As we already know these
; 1. How to write a bootable sector
; 2. How to init GDT
; 3. How to jump into protect mode
; 4. How to jump to lower privilege level
; 5. How to use call gate to access higer privilege level code
;
; So, we will strip unuseful code comparing to memory paging function

%include	"macros.inc"

org	0100h ; nasm directive - relocate label address by 0x0100

; Hard code page directory and page table address
PAGE_DIR_BASE	equ	200000h
PAGE_TABLE_BASE	equ	201000h

jmp	BEGIN	

[section .gdt]
; Global Descriptor Table
GDT_START:
			;    Base Address     Segment Limit      Attributes
GDT_DUMMY:	Descriptor	0,		0,			0 			; First descriptor is not used
DESC_CODE32:	Descriptor     	0,		0ffffh,   		DA_32BIT_EXE_ONLY_CODE
DESC_CODE16:	Descriptor	0,		0ffffh,			DA_16BIT_EXE_ONLY_CODE 
DESC_STACK:	Descriptor	0,		TopOfStack,		DA_32BIT_READ_WRITE_STACK
DESC_DATA:	Descriptor	0,		DataLen - 1,		DA_READ_WRITE_DATA 
DESC_VIDEO:	Descriptor	0B8000h,  	0ffffh,     		DA_READ_WRITE_DATA 
DESC_PAGE_DIR:	Descriptor	PAGE_DIR_BASE,	4095, 			DA_READ_WRITE_DATA
DESC_PAGE_TBL:	Descriptor	PAGE_TABLE_BASE,1023,			DA_READ_WRITE_DATA | DA_GRAN_4KB

; Return from 32bit mode, we need a proper selector loading into ds, es, ss etc
DESC_WORK_AROUND Descriptor	0,		0ffffh,			DA_READ_WRITE_DATA 

; GDT end

GDTLen 		equ	$ - GDT_DUMMY
GDTPtr		dw	GDTLen - 1
		dd	0	

; Selectors
SelectorCode32		equ	DESC_CODE32 - GDT_DUMMY
SelectorCode16		equ	DESC_CODE16 - GDT_DUMMY
SelectorData		equ	DESC_DATA - GDT_DUMMY 
SelectorStack		equ	DESC_STACK - GDT_DUMMY
SelectorVideo		equ	DESC_VIDEO - GDT_DUMMY
SelectorWorkAround	equ	DESC_WORK_AROUND - GDT_DUMMY
SelectorPageDir		equ	DESC_PAGE_DIR - GDT_DUMMY
SelectorPageTbl		equ	DESC_PAGE_TBL - GDT_DUMMY

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

; reading memory info
call	ReadingMemoryInfo

; init GDT loading info
xor	eax, eax
mov	ax, cs
shl	eax, 4
add	eax, GDT_START 
mov	dword	[GDTPtr + 2], eax

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

; -------------------- Auxilary Functions ---------------------

ReadingMemoryInfo:
mov	ebx, 0
mov	di, MemChkBuf
.loop:
mov	eax, 0E820h
mov	ecx, 20
mov	edx, 0534D4150h
int	15h
jc	MEM_CHK_FAILED
add	di, 20
inc	byte	[dbTotalAdrs]
cmp	ebx, 0
jne	.loop
jmp	MEM_CHK_OK
MEM_CHK_FAILED:
mov	byte	[dbTotalAdrs], 0
MEM_CHK_OK:

ret

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

; Close memory paging
; Close protect mode
mov	eax, cr0
and	eax, 7FFFFFFEh
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
ddCursorPosition	dd	0
OffsetCursorPosition	equ	ddCursorPosition - $$
szReturn		db	0Ah,0
OffsetReturn		equ	szReturn - $$
PMMessage:	db	"In Protect Mode now. ^-^",0
OffsetMessage	equ	PMMessage - $$
StrTest:	db	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",0
OffsetStrTest	equ	StrTest - $$
MemChkBuf	times	256	db	0
OffsetMemChkBuf	equ	MemChkBuf - $$
ddMemSize	dd	0
OffsetMemSize	equ	ddMemSize - $$
dbTotalAdrs	db	0
OffsetTotalAdrs	equ	dbTotalAdrs - $$
ddBaseLow	dd	0
OffsetBaseLow	equ	ddBaseLow - $$
ddLenLow	dd	0
OffsetLenLow	equ	ddLenLow - $$
szRamSize	db	"Ram size:",0
OffsetRamSize	equ	szRamSize - $$
szMemInfoHeader	db	"BaseAddrL BaseAddrH LengthLow LengthHigh    Type",0
szMemInfoHeaderOffset	equ	szMemInfoHeader - $$
DataLen		equ	$ - SEG_DATA

[section .stack]
align 32
[bits 32]
SEG_STACK:
times	512	db	0
TopOfStack	equ	$ - SEG_STACK - 1

[section .code32]
align 32
[bits 32]
SEG_CODE32:
mov	ax, SelectorData
mov	ds, ax
mov	es, ax
mov	ax, SelectorStack
mov	ss, ax
mov	esp, TopOfStack
mov	ax, SelectorVideo
mov	gs, ax

mov	al, 'L'
call	DisplayAL
call	DisplayReturn

; Init Memory Paging
call 	SetupPaging

push	OffsetMessage
call	DisplayStr
add	esp, 4
call	DisplayReturn

call	DisplayMemSize

jmp	SelectorCode16:0	

SetupPaging:
mov	ax, SelectorPageDir
mov	es, ax
mov	ecx, 1024
xor	edi, edi
xor	eax, eax
mov	eax, PAGE_TABLE_BASE | PG_PRESENT | PG_US_USER | PG_READ_WRITE
.1:
stosd
add	eax, 4096
loop	.1

mov	ax, SelectorPageTbl
mov	es, ax
mov	ecx, 1024 * 1024
xor	edi, edi
xor	eax, eax
mov	eax, PG_PRESENT | PG_US_USER | PG_READ_WRITE
.2:
stosd
add	eax, 4096
loop	.2

mov	eax, PAGE_DIR_BASE
mov	cr3, eax
mov	eax, cr0
or	eax, 80000000h
mov	cr0, eax
jmp	short	.3
.3:
nop
nop
nop

ret

DisplayMemSize:
push	szMemInfoHeaderOffset
call	DisplayStr
add	esp, 4
call	DisplayReturn

push	esi
push	edi
push	ecx
push	ebx
push	edx

mov	esi, OffsetMemChkBuf
mov	ecx, [OffsetTotalAdrs]

.loop:

;------ Display Low Base Address ------
push	dword	[esi]
call	DisplayInt
pop	ebx	

call	DisplayBlank

add	esi, 4

;------ Display High Base Address -------
push	dword	[esi]
call	DisplayInt
add	esp, 4

call 	DisplayBlank

add	esi, 4

;------- Display Low Length --------
push	dword	[esi]
call	DisplayInt
pop	edx

call	DisplayBlank

add	esi, 4

;------ Display High Length --------
push	dword	[esi]
call	DisplayInt
add	esp, 4

call	DisplayBlank

add	esi, 4

;------ Display Address Interval Type ------
push	dword	[esi]
call	DisplayInt
add	esp, 4

call	DisplayReturn

cmp	dword	[esi], 1
jne	.1
mov	eax, ebx 
add	eax, edx	
cmp	eax, [OffsetMemSize]
jb	.1
mov	[OffsetMemSize], eax	

add	esi, 4

.1:
loop	.loop

call	DisplayReturn
push	OffsetRamSize
call	DisplayStr
add	esp, 4

push	dword	[OffsetMemSize]
call	DisplayInt
add	esp, 4	

pop	edx
pop	ebx
pop	ecx
pop	edi
pop	esi

ret

;------------------------- Refer Library --------------------------
%include	"display.inc"
