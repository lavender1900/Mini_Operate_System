[section .data]
cursor_pos	dd	0	; Screen cursor position

[section .text]

global	cursor_pos
global	disp_str

; Display String which ended in '0', String offset passed by stack
disp_str:
push	ebp
mov	ebp, esp
push	ebx
push	eax
push	edi
push	esi
mov	dword	edi, [cursor_pos]
mov	dword	esi, [ebp + 8]

xor	eax, eax
cld
mov	ah, 0Fh
.loop:
lodsb
test	al, al 
jz	.end

; is Return character ?
cmp	al, 0Ah
jnz	.normal

mov	eax, edi
mov	bl, 160
div	bl
and	eax, 0FFh
inc	eax
mov	bl, 160
mul	bl
mov	edi, eax

jmp	.loop

.normal:
mov	[gs:edi], ax
add	edi, 2
jmp	.loop

.end:
mov	[cursor_pos], edi

pop	esi
pop	edi
pop	eax
pop	ebx
pop	ebp

ret
