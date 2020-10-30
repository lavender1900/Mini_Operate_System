org	07c00h
mov	ax, cs
mov	ds, ax
mov	es, ax
call 	DisplayString
jmp	$ ; Infinite loop

DisplayString:
mov	ax, BootMessage
mov	bp, ax
mov	cx, 16
mov	ax, 01301h
mov	bx, 0ch
mov	dl, 0
int	10h
ret

BootMessage:	db	"Hello, OS world!" ; Total 16 characters
times	510 - ($ - $$)	db	0
dw	0xaa55	; Bootable Device Indicator
