; Memory operations related

;-------- Memory Copy Function ----------
; C style is like MemCpy(void* srcPtr, void* destPtr, int copyLen)
;
; Copy unit is byte
; Function will consider DS:srcPtr points to src, ES:destPtr points to dest
MemCpy:
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
