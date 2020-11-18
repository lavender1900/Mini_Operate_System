%include	"kconst.inc"

NR_get_ticks		equ	0
INT_SYS_CALL		equ	0x70

global	get_ticks

[section .text]
align 32
[bits 32]
get_ticks:
mov	eax, NR_get_ticks
int	INT_SYS_CALL
ret
