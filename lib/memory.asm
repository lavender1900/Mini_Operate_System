
global	kmemcpy
global	kmemset
global	kmemcmp

[section .text]
align 32
[bits 32]
;***************** Memory Copy Function ******************
; C style is like kmemcpy(void* srcPtr, void* destPtr, int copyLen)
;
; Copy unit is byte
; Function will consider DS:srcPtr points to src, ES:destPtr points to dest
kmemcpy:
push	ebp
mov	ebp, esp
push	ecx
push	esi
push	edi
push	ebx

mov	esi, [ebp + 8]
mov	edi, [ebp + 12]
mov	ecx, [ebp + 16]
mov	ebx, ecx
and	ebx, 03h
shr	ecx, 2		; divide by 4, as 4bytes form 1 dword
test	ebx, ebx
jz	.perfect
inc	ecx
.perfect:
cld

.1:
mov	dword	eax, [esi]
stosd
add	esi, 4
loop	.1

pop	ebx
pop	edi
pop	esi
pop	ecx
pop	ebp

ret

;********************* kmemset(void* dest, u8 t, int times) ********************
kmemset:
push	ebp
mov	ebp, esp
push	ecx
push	eax
push	edi

mov	ecx, [ebp + 16]	; loop times
mov	edi, [ebp + 8]	; dest
xor	eax, eax
mov	al, [ebp + 12]	; value to set

.1:
stosb
loop	.1

pop	edi
pop	eax
pop	ecx
pop	ebp

ret

;******************* kmemcmp(void* src, void* dest, int maxCompareChars) ********************
kmemcmp:
push	ebp
mov	ebp, esp
push	ecx
push	edi
push	esi
push	ebx

mov	esi, [ebp + 8] 	;src
mov	edi, [ebp + 12]	;dest 
mov	ecx, [ebp + 16]	;max compare chars
 
xor	ebx, ebx
.1:
mov	bl, [esi];
cmp	byte bl, [edi]
jne	.2	

test	bl, bl
jz	.3

sub	ecx, 1
test	ecx, ecx
jz	.3

add	esi, 1
add	edi, 1
jmp	.1

.2:
mov	eax, 1
jmp	.back

.3:
mov	eax, 0

.back:
pop	ebx
pop	esi
pop	edi
pop	ecx

ret
