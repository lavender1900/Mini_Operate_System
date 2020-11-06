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

; Hard coded Page directory and page table base address
PAGE_DIR_BASE0		equ	200000h
PAGE_TABLE_BASE0	equ	201000h

PAGE_DIR_BASE1		equ	300000h
PAGE_TABLE_BASE1	equ	301000h

LinearAddrDemo		equ	401000h
ProcFoo			equ	401000h
ProcBar			equ	501000h
ProcPagingDemo		equ	380000h

%include	"include/macros.inc"

org	0100h ; nasm directive - relocate label address by 0x0100

jmp	BEGIN	

[section .gdt]
; Global Descriptor Table
GDT_START:
			;    Base Address     Segment Limit      Attributes
GDT_DUMMY:	Descriptor	0,		0,			0 			; First descriptor is not used
DESC_CODE32:	Descriptor     	0,		0ffffh,   		DA_32BIT_EXE_READ_CODE
DESC_CODE16:	Descriptor	0,		0ffffh,			DA_16BIT_EXE_ONLY_CODE 
DESC_STACK:	Descriptor	0,		TopOfStack,		DA_32BIT_READ_WRITE_STACK
DESC_DATA:	Descriptor	0,		DataLen - 1,		DA_READ_WRITE_DATA 
DESC_VIDEO:	Descriptor	0B8000h,  	0ffffh,     		DA_READ_WRITE_DATA 
DESC_FLAT_C	Descriptor	0,		0fffffh,		DA_32BIT_EXE_ONLY_CODE | DA_GRAN_4KB
DESC_FLAT_RW	Descriptor	0,		0fffffh,		DA_READ_WRITE_DATA | DA_GRAN_4KB			


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
SelectorFlatCode	equ	DESC_FLAT_C - GDT_DUMMY
SelectorFlatRw		equ	DESC_FLAT_RW - GDT_DUMMY
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
mov	di, _MemChkBuf
.loop:
mov	eax, 0E820h
mov	ecx, 20
mov	edx, 0534D4150h
int	15h
jc	MEM_CHK_FAILED
add	di, 20
inc	dword	[_dwTotalAdrs]
cmp	ebx, 0
jne	.loop
jmp	MEM_CHK_OK
MEM_CHK_FAILED:
mov	dword	[_dwTotalAdrs], 0
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
; ---------- Important system variables -----------
SPValueInRealMode	dw	0
_dwCursorPosition	dd	0
dwCursorPosition	equ	_dwCursorPosition - $$
_dwPageTableNumber	dd	0
dwPageTableNumber	equ	_dwPageTableNumber - $$
_MemChkBuf times	256	db	0
MemChkBuf		equ	_MemChkBuf - $$
_dwMemSize		dd	0
dwMemSize		equ	_dwMemSize - $$
_dwTotalAdrs		dd	0
dwTotalAdrs		equ	_dwTotalAdrs - $$

; ---------- String variables ------------
_szReturn		db	0Ah,0
szReturn		equ	_szReturn - $$
_szPMMessage:		db	"In Protect Mode now. ^-^",0Ah,0
szPMMessage		equ	_szPMMessage - $$
_szRamSize		db	"Ram size:",0
szRamSize		equ	_szRamSize - $$
_szMemInfoHeader		db	"BaseAddrL BaseAddrH LengthLow LengthHigh    Type",0Ah,0
szMemInfoHeader		equ	_szMemInfoHeader - $$

_szFoo			db	"Hello there, it's foo!",0Ah,0
szFoo			equ	_szFoo - $$
_szBar			db	"Ah ha, magic made me bar!!!",0Ah,0
szBar			equ	_szBar - $$	

DataLen			equ	$ - SEG_DATA

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

push	8
call	DisplayMultiReturn
add	esp, 4

push	szPMMessage	
call	DisplayStr
add	esp, 4

; Show memory distribution
call	DisplayMemSize
call	DisplayReturn

; Init Memory Paging
call 	SetupPaging

call	PagingDemo

jmp	SelectorCode16:0	

; init paging according to actual memory size
SetupPaging:
; calculate
push	edx
push	ebx
xor	edx, edx
mov	eax, [dwMemSize]
mov	ebx, 400000h 		; 4MB - Mapped memory size to a page directory entry
div	ebx
mov	ecx, eax		; ecx stores the quotient
test	edx, edx		; edx stores the remainder
jz	.noremainder
inc	ecx
.noremainder:
mov	dword	[dwPageTableNumber], ecx

; init page directory entries
mov	ax, SelectorFlatRw
mov	es, ax
mov	edi, PAGE_DIR_BASE0
xor	eax, eax
mov	eax, PAGE_TABLE_BASE0 | PG_PRESENT | PG_US_USER | PG_READ_WRITE
.1:
stosd
add	eax, 4096
loop	.1

; init page table entries
mov	eax, [dwPageTableNumber]
mov	ebx, 1024
mul	ebx
mov	ecx, eax		; Page table entry number
xor	eax, eax
mov	edi, PAGE_TABLE_BASE0
mov	eax, PG_PRESENT | PG_US_USER | PG_READ_WRITE
.2:
stosd
add	eax, 4096
loop	.2

mov	eax, PAGE_DIR_BASE0
mov	cr3, eax
mov	eax, cr0
or	eax, 80000000h
mov	cr0, eax
jmp	short	.3
.3:
nop
nop
nop

pop	ebx
pop	edx

ret

DisplayMemSize:
push	szMemInfoHeader
call	DisplayStr
add	esp, 4

push	esi
push	edi
push	ecx
push	ebx
push	edx

mov	esi, MemChkBuf
mov	ecx, [dwTotalAdrs]

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
cmp	eax, [dwMemSize]
jb	.1
mov	[dwMemSize], eax	

.1:
add	esi, 4

loop	.loop

call	DisplayReturn
push	szRamSize
call	DisplayStr
add	esp, 4

push	dword	[dwMemSize]
call	DisplayInt
add	esp, 4	

pop	edx
pop	ebx
pop	ecx
pop	edi
pop	esi

ret

PagingDemo:
mov	ax, SelectorFlatRw
mov	es, ax
mov	ax, SelectorCode32
mov	ds, ax

; MemCpy(void* srcPtr, void* destPtr, int length)
push	LenFoo
push	ProcFoo
push	OffsetFoo
call	MemCpy
add	esp, 12

push	LenBar
push	ProcBar
push	OffsetBar
call	MemCpy
add	esp, 12

push	LenPagingDemo
push	ProcPagingDemo
push	OffsetPagingDemoProc
call	MemCpy
add	esp, 12

mov	ax, SelectorData
mov	ds, ax
mov	es, ax

call	SelectorFlatCode:ProcPagingDemo
call	PagingSwitch
call	SelectorFlatCode:ProcPagingDemo

ret

PagingDemoProc:
OffsetPagingDemoProc	equ	PagingDemoProc - $$
mov	eax, LinearAddrDemo
call	eax
retf

LenPagingDemo	equ	$ - PagingDemoProc

foo:
OffsetFoo	equ	foo - $$
push	ebx

mov	ah, 0Ch
mov	al, 'F'
mov	ebx, [dwCursorPosition]
mov	[gs:ebx], ax
add	ebx, 2
mov	al, 'O'
mov	[gs:ebx], ax
add	ebx, 2
mov	[gs:ebx], ax
add	ebx, 2

mov	[dwCursorPosition], ebx

pop	ebx

ret

LenFoo	equ	$ - foo

bar:
OffsetBar	equ	bar - $$
push	ebx

mov	ah, 0Ch
mov	al, 'B'
mov	ebx, [dwCursorPosition]
mov	[gs:ebx], ax
add	ebx, 2
mov	al, 'A'
mov	[gs:ebx], ax
add	ebx, 2
mov	al, 'R'
mov	[gs:ebx], ax
add	ebx, 2
mov	[dwCursorPosition], ebx

pop	ebx

ret

LenBar	equ	$ - bar

PagingSwitch:
mov	ax, SelectorFlatRw
mov	es, ax
mov	edi, PAGE_DIR_BASE1
xor	eax, eax
mov	eax, PAGE_TABLE_BASE1 | PG_PRESENT | PG_US_USER | PG_READ_WRITE
mov	ecx, [dwPageTableNumber]
.1:
stosd
add	eax, 4096
loop	.1

mov	eax, [dwPageTableNumber]
mov	ebx, 1024
mul	ebx
mov	ecx, eax
mov	edi, PAGE_TABLE_BASE1
xor	eax, eax
mov	eax, PG_PRESENT | PG_US_USER | PG_READ_WRITE

.2:
stosd
add	eax, 4096
loop	.2

mov	eax, LinearAddrDemo
shr	eax, 22
mov	ebx, 4096
mul	ebx
mov	ecx, eax
mov	eax, LinearAddrDemo
shr	eax, 12
and	eax, 03FFh
mov	ebx, 4
mul	ebx
add	eax, ecx
add	eax, PAGE_TABLE_BASE1
mov	dword	[es:eax], ProcBar | PG_PRESENT | PG_US_USER | PG_READ_WRITE

mov	eax, PAGE_DIR_BASE1
mov	cr3, eax
jmp	short	.3

.3:
nop
nop
nop

ret

;------------------------- Refered Library --------------------------
%include	"include/display.inc"
%include	"include/memory.inc"
