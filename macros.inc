%macro	Descriptor	3 ; Three parameters
	dw	%2 & 0FFFFh ; Segment Limit 1
	dw	%1 & 0FFFFh ; Segment Base Addr 1
	db	(%1 >> 16) & 0FFh ; Segment Base Addr 2
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh) ; Attributes + Segment Limit 2
	db	(%1 >> 24) & 0FFh
%endmacro
