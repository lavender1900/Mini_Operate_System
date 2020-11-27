%include	"kconst.inc"

global	write
global	printx	
global	sendrec
global	sendint

[section .text]
align 32
[bits 32]
write:
mov	eax, NR_write
mov	ebx, [esp+4]
mov	ecx, [esp+8]
int	INT_SYS_CALL
ret

printx:
mov	eax, NR_printx
mov	ebx, [esp+4]
mov	ecx, [esp+8]
int	INT_SYS_CALL
ret

sendrec:
mov	eax, NR_sendrec
mov	ebx, [esp + 4]		; function
mov	ecx, [esp + 8]		; src_dest
mov	edx, [esp + 12]		; p_msg
int	INT_SYS_CALL
ret

sendint:
mov	eax, NR_sendint
mov	ebx, [esp + 4]
mov	ecx, [esp + 8]
mov	edx, [esp + 12]
int	INT_SYS_CALL
ret
