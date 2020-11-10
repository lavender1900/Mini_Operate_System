[section .text]
align 32
[bits 32]
global	_start

_start:
xchg	bx, bx
mov	ah, 0Fh
mov	al, 'K'
mov	[gs:((80 * 20 + 0 ) * 2)], ax
jmp	$
