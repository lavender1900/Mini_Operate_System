; We will write a FAT12 compatible boot sector

%ifdef	_BOOT_DEBUG_
	org	0100h
%else
	org	07c00h
%endif

%include	"include/fat12.inc"

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

mov	dh, 0
call	DispStr

mov	word 	[wSectorNo], SectorNoOfRootDirectory
SEARCH_IN_ROOT_DIR_BEGIN:
cmp	word	[wRootDirSizeForLoop], 0
jz	NO_LOADERBIN
dec	word	[wRootDirSizeForLoop]
mov	ax, BaseOfLoader
mov	es, ax
mov	bx, OffsetOfLoader
mov	ax, [wSectorNo]
mov	cl, 1
call	ReadSector

mov	si, szLoaderFileName
mov	di, OffsetOfLoader
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
mov	dh, 2
call	DispStr

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
mov	ax, BaseOfLoader
mov	es, ax
mov	bx, OffsetOfLoader
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
jmp	BaseOfLoader:OffsetOfLoader	


; 1.44MB floppy has 2 heads, 80 tracks per head, 18 sectors per track
; total 2 * 80 * 18 * 512 = 1.44MB
ReadSector:
push	bp
mov	bp, sp
sub	esp, 2

mov	byte	[bp-2], cl	; save sector number to be read
push	bx
mov	bl, [BPB_SecPerTrack]	; (sector no) / 18
div	bl
inc	ah			; Sector no is start from 1, NOT 0
mov	cl, ah
mov	dh, al
shr	al, 1
mov	ch, al			; Cylinder no
and	dh, 1
pop	bx

mov	dl, [BS_DriverNum]
.GoOnReading:
mov	ah, 2
mov	al, byte [bp-2]
int	13h
jc	.GoOnReading

add	esp, 2
pop	bp

ret

GetFATEntry:
push	es
push	bx
push	ax
mov	ax, BaseOfLoader
sub	ax, 100h
mov	es, ax		; Keep 4KB space before BaseOfLoader:0100h
pop	ax
mov	byte	[bOdd], 0
mov	bx, 3
mul	bx
mov	bx, 2
div	bx
cmp	dx, 0
jz	EVEN
mov	byte	[bOdd], 1

EVEN:
xor	dx, dx
mov	bx, [BPB_BytesPerSec]
div	bx
push	dx
mov	bx, 0
add	ax, SectorNoOfFAT1
mov	cl, 2
call	ReadSector

pop	dx
add	bx, dx
mov	ax, [es:bx]
cmp	byte [bOdd], 1
jnz	EVEN_2
shr	ax, 4
EVEN_2:
and	ax, 0FFFh

GAT_ENTRY_OK:
pop	bx
pop	es
ret

DispStr:
mov	ax, MessageLength
mul	dh
add	ax, szBootMessage
mov	bp, ax
mov	ax, ds
mov	es, ax
mov	cx, MessageLength
mov	ax, 01301h
mov	bx, 07h
mov	dl, 0
int	10h

ret


wSectorNo	dw	0
wRootDirSizeForLoop	dw	RootDirSectors	
bOdd		db	0

szLoaderFileName	db	"LOADER  BIN",0
MessageLength		equ	9
szBootMessage:		db	"Booting  "
szReadyMessage:		db	"Ready.   "
szNoLoader:		db	"No Loader"
szLoaded:		db	"Loaded!!!"

BaseOfLoader		equ	9000h
OffsetOfLoader		equ	100h
RootDirSectors		equ	14
SectorNoOfRootDirectory	equ	19
SectorNoOfFAT1		equ	1
DeltaSectorNo		equ	17

times	510 - ($ - $$) db	0
dw	0xaa55
