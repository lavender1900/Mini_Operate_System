; We will write a FAT12 compatible boot sector

org	07c00h

jmp	BOOT_START
nop

%include	"include/fat12header.inc"

BOOT_START:
mov	ax, cs
mov	ds, ax
mov	ss, ax
mov	sp, 7c00h
mov	es, ax
mov	ax, 0B800h
mov	gs, ax

SearchForLoader:
xor	ah, ah
xor	dl, dl
int	13h		; Floppy reset

; clear screen
mov	ax, 600h
mov	bx, 700h
mov	cx, 0
mov	dx, 0184fh
int	10h

push	0
mov	dh, 0
call	DispStrRealMode
add	sp, 2 

mov	word 	[wSectorNo], SectorNoOfRootDirectory
SEARCH_IN_ROOT_DIR_BEGIN:
cmp	word	[wRootDirSizeForLoop], 0
jz	NO_LOADERBIN
dec	word	[wRootDirSizeForLoop]
mov	ax, BaseOfFile
mov	es, ax
mov	bx, OffsetOfFile
mov	ax, [wSectorNo]
mov	cl, 1
call	ReadSector

mov	si, szLoaderFileName
mov	di, OffsetOfFile
cld
mov	dx, 10h

SEARCH_FOR_LOADERBIN:
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
mov	si, szLoaderFileName
jmp	SEARCH_FOR_LOADERBIN 

GO_TO_NEXT_SECTOR_IN_ROOT_DIR:
add	word [wSectorNo], 1
jmp	SEARCH_IN_ROOT_DIR_BEGIN

NO_LOADERBIN:
push	0
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
jmp	BaseOfFile:OffsetOfFile	

; include ReadSector, GetFatEntry functions
%include 	"include/fat12read.inc"

szLoaderFileName	db	"LOADER  BIN",0
MessageLength		equ	9
szBootMessage:		db	"Booting  "
szNoLoader:		db	"No Loader"

BaseOfFile		equ	9000h
OffsetOfFile		equ	100h

times	510 - ($ - $$) db	0
dw	0xaa55
