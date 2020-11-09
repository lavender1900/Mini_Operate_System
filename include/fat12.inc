; Here we create a FAT12 file system header
; The reason wo do this is to make os.img recognizable by
; freedos and linux. Then we can easily copy files to os.img as it
; is a normal FAT12 formatted device. 
; Meanwhile we can running bochs anywhere anytime using os.img directly as the bootable device

; BS - Boot Sector
; BPB - BIOS Parameter Block

;-------- FAT12 Format ---------
; 0  | Boot Sector |
;----------------------------
; 1  |             |
;    |    FAT1     |
; 9  |             |
;----------------------------
; 10 |             | 
;    |    FAT2     |
; 18 |             |
;----------------------------
;    |             |
;    | Root Directory
;    | Not Fixed Length
;    |  .......    |
;----------------------------
;    |             |
;    | Data Area   |
;    | Not Fixed Length
;2879| ........    |
;----------------------------
 

jmp	short	BOOT_START
nop

BS_OEMNAME		db	'Lavender'
BPB_BytesPerSec		dw	512		; Bytes per sector
BPB_SecPerClus		db	1		; Sectors per cluster
BPB_ReservedSecCnt	dw	1		; Sectors boot code used
BPB_NumFATs		db	2		; Number of FAT table
BPB_RootEntryCnt	dw	224		; Root directory entry numbers
BPB_TotalSec		dw	2880		; Total sector numbers
BPB_Media		db	0xF0		; Media descriptor
BPB_SecPerFAT		dw	9		; Sectors per FAT table
BPB_SecPerTrack		dw	18		; Sectors per Track
BPB_NumHeads		dw	2		; Heads
BPB_HiddenSec		dd	0		; Hidden sectors
BPB_TotalSec32		dd	0		; 
BS_DriverNum		db	0		; Driver number of int 13 ?
BS_Reserved		db	0
BS_BootSig		db	29h
BS_VolID		dd	0
BS_ValLabel		db	'Lavender0.1'
BS_FileSysType		db	'FAT12   '	; File System
