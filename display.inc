;
; Include file for various display functions
;

; Display a blank space
DisplayBlank:
inc	dword	[OffsetCursorPosition]
inc	dword	[OffsetCursorPosition]

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
mov	edi, [OffsetCursorPosition]
mov	[gs:edi], ax
add	edi, 2 
mov	[OffsetCursorPosition], edi
pop	edi

ret


; Display Hex value in AL
DisplayAL:
push	ecx
push	edx
push	edi

mov	edi, [OffsetCursorPosition]

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

mov	dword	[OffsetCursorPosition], edi
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
mov	dword	edi, [OffsetCursorPosition]
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
mov	[OffsetCursorPosition], edi

pop	esi
pop	edi
pop	eax
pop	ebx
pop	ebp

ret


; Display Return
DisplayReturn:
push	OffsetReturn	
call	DisplayStr
add	esp, 4

ret
