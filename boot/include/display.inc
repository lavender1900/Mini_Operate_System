;
; Include file for various display functions
;

; Display a blank space
DisplayBlank:
inc	dword	[dwCursorPosition]
inc	dword	[dwCursorPosition]

ret

; Display an Integer, Hex format 
DisplayInt:
mov	eax, [esp + 4]
shr	eax, 24
call	DisplayAL

mov	eax, [esp + 4]
shr	eax, 16
call	DisplayAL

mov	eax, [esp + 4]
shr	eax, 8
call	DisplayAL

mov	eax, [esp + 4]
call	DisplayAL

mov	ah, 07h		; 0000b - black background, 0111b - gray chars 
mov	al, 'h'
push	edi
mov	edi, [dwCursorPosition]
mov	[gs:edi], ax
add	edi, 2 
mov	[dwCursorPosition], edi
pop	edi

ret


; Display Hex value in AL
DisplayAL:
push	ecx
push	edx
push	edi

mov	edi, [dwCursorPosition]

mov	ecx, 2
mov	dl, al
shr	al, 4
.begin:
and	al, 0Fh
cmp	al, 9 
ja	.1
add	al, '0'
jmp	.2

.1:
sub	al, 10
add	al, 'A'

.2:
mov	ah, 0Ch
mov	[gs:edi], ax
add	edi, 2
mov	al, dl
loop	.begin

mov	dword	[dwCursorPosition], edi
pop	edi
pop	edx
pop	ecx

ret

; Display String which ended in '0', String offset passed by stack
DisplayStr:
push	ebp
mov	ebp, esp
push	ebx
push	eax
push	edi
push	esi
mov	dword	edi, [dwCursorPosition]
mov	dword	esi, [ebp + 8]

xor	eax, eax
cld
mov	ah, 0Ch
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
mov	[dwCursorPosition], edi

pop	esi
pop	edi
pop	eax
pop	ebx
pop	ebp

ret


; Display Return
DisplayReturn:
push	szReturn	
call	DisplayStr
add	esp, 4

ret

; Display Multiple Return
DisplayMultiReturn:
push	ebp
mov	ebp, esp
push	ecx
mov	dword	ecx, [ebp + 8]
.1:
call	DisplayReturn
loop	.1

pop	ecx
pop	ebp

ret
