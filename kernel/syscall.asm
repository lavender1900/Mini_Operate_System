%include	"kconst.inc"

global	get_ticks
global	write

[section .text]
align 32
[bits 32]
get_ticks:
mov	eax, NR_get_ticks
int	INT_SYS_CALL
ret

write:
mov	eax, NR_write
mov	ebx, [esp+4]
mov	ecx, [esp+8]
int	INT_SYS_CALL
ret
