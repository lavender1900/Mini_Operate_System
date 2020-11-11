
;************** Code Part *********************

; 1.44MB floppy has 2 heads, 80 tracks per head, 18 sectors per track
; total 2 * 80 * 18 * 512 = 1.44MB
;------------ Read Sector Start ----------------------
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
;----------- Read Sector End ---------------

;------------ Read FAT Entry Start -------------
GetFATEntry:
push	es
push	bx
push	ax
mov	ax, BaseOfFile
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
;----------- Read FAT Entry End -------------------

;----------- Display String Start ----------------
DispStrRealMode:
push	si	
mov	si, sp
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
add	dh, [si+4]	; Start line
int	10h

pop	si
ret
;----------- Display String End ------------------

;**************  Data Part *********************
wSectorNo		dw	0
wRootDirSizeForLoop	dw	RootDirSectors
bOdd			db	0
