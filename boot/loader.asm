; Here we write a loader in order to load Kernel.bin to memory
; Then jump to kernel code, finish our booting progress
; We define a flat code segment, a flat data segment and a video segment

org	0100h		; Loader.bin will be loaded to ES:0100h by Boot.bin

jmp	LOADER_START

%include	"macros.inc"

;************************** Global Descriptor Table *********************************
GDT_START:
				; Base Address	  Segment Limit	      Attributes
GDT_DUMMY:		Descriptor	0,		0,		0
DESC_FLAT_C:		Descriptor	0,		0fffffh,	DA_32BIT_EXE_ONLY_CODE | DA_GRAN_4KB
DESC_FLAT_RW:		Descriptor	0,		0fffffh,	DA_READ_WRITE_DATA | DA_GRAN_4KB 
DESC_VIDEO:		Descriptor	0B8000h,	0ffffh,		DA_READ_WRITE_DATA | DA_DPL3

GDTLen		equ		$ - GDT_START
GDTPtr		dw		GDTLen - 1				; GDT Limit
		dd		BaseOfLoaderPhysicAddr + GDT_START	; GDT Base Address	

SelectorFlatC		equ	DESC_FLAT_C - GDT_DUMMY
SelectorFlatRw		equ	DESC_FLAT_RW - GDT_DUMMY
SelectorStack		equ	DESC_FLAT_RW - GDT_DUMMY
SelectorVideo		equ	(DESC_VIDEO - GDT_DUMMY) | SA_RPL3 

%include	"fat12header.inc"

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

;******************** Try to find kernel.bin in floppy, if found load kernel.bin to memory then jump to it ****************************
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

;*********************** Reading Memory Distribution Info *******************************
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
%include 	"fat12read.inc"

;****************************** Data Area *********************************
StackSpace:	times	1024	db	0
TopOfStack	equ	BaseOfLoaderPhysicAddr + $

szKernelFileName	db	"KERNEL  BIN",0
MessageLength		equ	14	
szBootMessage:		db	"Loading Kernel"
szNoKernel		db	"No Kernel     "
szKernelLoaded:		db	"Kernel Loaded "

_dwPageTableNumber	dd	0
dwPageTableNumber	equ	BaseOfLoaderPhysicAddr + _dwPageTableNumber 
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
BaseOfKernelPhysicAddr	equ	080000h
KernelEntryPointPhyAddr	equ	030400h
PAGE_DIR_BASE		equ	100000h		; 1MB
PAGE_TABLE_BASE		equ	101000h		; 1MB + 4KB

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

call	SetupPaging

call	MoveKernelToElfSpecified

jmp	SelectorFlatC:KernelEntryPointPhyAddr

;********************* Display Memory And Caculate maximum available memory ***********************
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

;************************** init paging according to actual memory size ********************************** 
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
mov	edi, PAGE_DIR_BASE
xor	eax, eax
mov	eax, PAGE_TABLE_BASE | PG_PRESENT | PG_US_USER | PG_READ_WRITE
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
mov	edi, PAGE_TABLE_BASE
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

pop	ebx
pop	edx

ret

;************************ Move kernel.bin from 080000h to the address specified by kernel.bin elf header ***************************
MoveKernelToElfSpecified:
xor	esi, esi
xor	ecx, ecx
mov	cx, word [BaseOfKernelPhysicAddr + 2Ch]		; ecx = ELFHeader->e_phnum (Program Header Number)	2Ch = 44, e_phum offset
mov	esi, [BaseOfKernelPhysicAddr + 1Ch]		; esi = ELFHeader->e_phoff (Program Header Offset)
add	esi, BaseOfKernelPhysicAddr
.begin:
mov	eax, [esi]
cmp	eax, 1		; Check program header type, wierd problems - GNU Stack segment Destination is at address 0, we should skip it
jnz	.no_action
push	dword [esi + 10h]		; Size
push	dword [esi + 08h]		; Destination - Segment will be copied to this address
mov	eax, [esi + 4h]			; First byte of Segment in Kernel.bin
add	eax, BaseOfKernelPhysicAddr 	; Source - Segment to be copied
push	eax

;---------------- We have specified 0x30400 as the program entry point, but it seems that there exists some segments locate on high address in ELF program headers ------------------------
cmp	dword [esi + 08h], 050000h
ja	.no_copy

;----------------- MemCpy(void* src, void* dest, int copyLen) ----------------
call	MemCpy

.no_copy:
add	esp, 12

.no_action:
add	esi, 20h		; Each program header has 0x20 bytes
dec	ecx
jnz	.begin

ret


%include	"display.inc"
%include	"memory.inc"
