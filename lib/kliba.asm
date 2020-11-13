
[section .text]

extern	cursor_pos

global	disp_str
global	disp_color_str
global	out_byte
global	in_byte

;*******************  Display String which ended in '0', String offset passed by stack ***********************
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


;******************* Colorfully Display String which ended in '0', String offset passed by stack ***********************
disp_color_str:
push	ebp
mov	ebp, esp
push	ebx
push	eax
push	edi
push	esi
mov	dword	edi, [cursor_pos]
mov	dword	esi, [ebp + 8]
mov	ah, [ebp + 12]

xor	eax, eax
cld
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



;***************** write byte to port *********************
out_byte:
push	ebp
mov	ebp, esp
push	edx
push	eax

mov	edx, [ebp + 8]
mov	al, [ebp + 12]
out	dx, al
nop
nop
nop
nop

pop	eax
pop	edx
pop	ebp

ret

;********************* Read byte from port ************************
in_byte:
push	ebp
mov	ebp, esp
push	edx

mov	edx, [ebp + 8]
xor	eax, eax
in	al, dx
nop
nop
nop
nop

pop	edx
pop	ebp

ret
