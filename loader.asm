; Here we write a loader in order to load Kernel.bin to memory
; Then jump to kernel code, finish our booting progress
; We define a flat code segment, a flat data segment and a video segment

org	0100h		; Loader.bin will be loaded to ES:0100h by Boot.bin

jmp	LOADER_START

%include	"include/macros.inc"

; Global Descriptor Table
GDT_START:
				; Base Address	  Segment Limit	      Attributes
GDT_DUMMY:		Descriptor	0,		0,		0
DESC_FLAT_C:		Descriptor	0,		0fffffh,	DA_32BIT_EXE_ONLY_CODE | DA_GRAN_4KB
DESC_FLAT_RW:		Descriptor	0,		0fffffh,	DA_READ_WRITE_DATA | DA_GRAN_4KB 
DESC_STACK:		Descriptor	0,		0fffffh,	DA_32BIT_READ_WRITE_STACK | DA_GRAN_4KB
DESC_VIDEO:		Descriptor	0B8000h,	0ffffh,		DA_READ_WRITE_DATA | DA_DPL3

GDTLen		equ		$ - GDT_START
GDTPtr		dw		GDTLen - 1				; GDT Limit
		dd		BaseOfLoaderPhysicAddr + GDT_START	; GDT Base Address	

SelectorFlatC		equ	DESC_FLAT_C - GDT_DUMMY
SelectorFlatRw		equ	DESC_FLAT_RW - GDT_DUMMY
SelectorStack		equ	DESC_STACK - GDT_DUMMY
SelectorVideo		equ	(DESC_VIDEO - GDT_DUMMY) | SA_RPL3 

%include	"include/fat12header.inc"

LOADER_START:
mov	ax, cs
mov	ds, ax
mov	es, ax
mov	ss, ax
mov	sp, BaseOfStack

push	2
mov	dh, 0
call	DispStrRealMode 	; Show "Loading..." string
add	sp, 2

xor	ah, ah
xor	dl, dl
int	13h		; Reset Floppy

mov	word 	[wSectorNo], SectorNoOfRootDirectory
SEARCH_IN_ROOT_DIR_BEGIN:
cmp	word	[wRootDirSizeForLoop], 0
jz	NO_KERNELBIN
dec	word	[wRootDirSizeForLoop]
mov	ax, BaseOfFile
mov	es, ax
mov	bx, OffsetOfFile
mov	ax, [wSectorNo]
mov	cl, 1
call	ReadSector

mov	si, szKernelFileName
mov	di, OffsetOfFile
cld
mov	dx, 10h

SEARCH_FOR_KERNELBIN:
cmp	dx, 0
jz	GO_TO_NEXT_SECTOR_IN_ROOT_DIR
dec	dx
mov	cx, 11
CMP_FILENAME:
cmp	cx, 0
jz	FILENAME_FOUND
dec	cx
lodsb
cmp	al, byte [es:di]
jz	GO_ON
jmp	FILENAME_NOT_MATCH

GO_ON:
inc	di
jmp	CMP_FILENAME

FILENAME_NOT_MATCH:
and	di, 0FFE0h
add	di, 20h
mov	si, szKernelFileName
jmp	SEARCH_FOR_KERNELBIN 

GO_TO_NEXT_SECTOR_IN_ROOT_DIR:
add	word [wSectorNo], 1
jmp	SEARCH_IN_ROOT_DIR_BEGIN

NO_KERNELBIN:
push	2
mov	dh, 1
call	DispStrRealMode
add	sp, 2

%ifdef	_BOOT_DEBUG_
mov	ax, 4c00h
int	21h		; back to dos
%else
jmp	$
%endif

FILENAME_FOUND:
mov	ax, RootDirSectors
and	di, 0FFE0h
add	di, 01Ah
mov	cx, word [es:di]
push	cx		; Data area sector no(start from 2)
add	cx, ax
add	cx, DeltaSectorNo
mov	ax, BaseOfFile
mov	es, ax
mov	bx, OffsetOfFile
mov	ax, cx			; Sector no to be read

GO_ON_LOADING_FILE:
push	ax
push	bx
mov	ah, 0Eh
mov	al, '.'
mov	bl, 0Fh
int	10h
pop	bx
pop	ax

mov	cl, 1
call	ReadSector
pop	ax
call	GetFATEntry
cmp	ax, 0FFFh
jz	FILE_LOADED
push	ax
mov	dx, RootDirSectors
add	ax, dx
add	ax, DeltaSectorNo
add	bx, [BPB_BytesPerSec]
jmp	GO_ON_LOADING_FILE

FILE_LOADED:
push	2
mov	dh, 2
call	DispStrRealMode
add	sp, 2

call	ReadingMemoryInfo

lgdt	[GDTPtr]
cli

in	al, 92h
or	al, 02h
out	92h, al

mov	eax, cr0
or	eax, 1
mov	cr0, eax

jmp	dword SelectorFlatC:(BaseOfLoaderPhysicAddr + PROTECT_START)

; Reading Memory Distribution Info
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

; include ReadSector, GetFatEntry functions
%include 	"include/fat12read.inc"

StackSpace:	times	1024	db	0
TopOfStack	equ	BaseOfLoaderPhysicAddr + $

szKernelFileName	db	"KERNEL  BIN",0
MessageLength		equ	14	
szBootMessage:		db	"Loading Kernel"
szNoKernel		db	"No Kernel     "
szKernelLoaded:		db	"Kernel Loaded "

_dwCursorPosition	dd	(80 * 8 + 0) * 2		; 8th line, 0 column
dwCursorPosition	equ	BaseOfLoaderPhysicAddr + _dwCursorPosition
_MemChkBuf times	256	db	0
MemChkBuf		equ	BaseOfLoaderPhysicAddr + _MemChkBuf
_dwMemSize		dd	0
dwMemSize		equ	BaseOfLoaderPhysicAddr + _dwMemSize
_dwTotalAdrs		dd	0
dwTotalAdrs		equ	BaseOfLoaderPhysicAddr + _dwTotalAdrs

_szReturn		db	0Ah,0
szReturn		equ	BaseOfLoaderPhysicAddr + _szReturn
_szPMMessage:		db	"In Protect Mode now. ^-^",0Ah,0
szPMMessage		equ	BaseOfLoaderPhysicAddr + _szPMMessage
_szRamSize		db	"Ram size:",0
szRamSize		equ	BaseOfLoaderPhysicAddr + _szRamSize
_szMemInfoHeader		db	"BaseAddrL BaseAddrH LengthLow LengthHigh    Type",0Ah,0
szMemInfoHeader		equ	BaseOfLoaderPhysicAddr + _szMemInfoHeader

BaseOfStack		equ	0100h
BaseOfFile		equ	08000h
OffsetOfFile		equ	0h
BaseOfLoaderPhysicAddr	equ	090000h	

[section .code32]
align 32
[bits 32]
PROTECT_START:
mov	ax, SelectorFlatRw
mov	ds, ax
mov	es, ax
mov	fs, ax
mov	ax, SelectorStack
mov	ss, ax
mov	esp, TopOfStack
mov	ax, SelectorVideo
mov	gs, ax

call	DisplayMemSize

jmp	$ 

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

%include	"include/display.inc"
