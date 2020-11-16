; Global/Local Segment Descriptor Struct
%macro	Descriptor	3 ; Three parameters
	dw	%2 & 0FFFFh ; Segment Limit 1
	dw	%1 & 0FFFFh ; Segment Base Addr 1 (first two bytes)
	db	(%1 >> 16) & 0FFh ; Segment Base Addr 1 (third byte)
	dw	((%2 >> 8) & 0F00h) | ((%3) & 0F0FFh) ; Attributes + Segment Limit 2
	db	(%1 >> 24) & 0FFh ; Segment Base Addr 2 (forth byte)
%endmacro

; Gate Descriptor Struct
%macro	Gate	4;
	dw	%2 & 0FFFFh
	dw	%1
	dw	(%3 & 1Fh) | (((%4) << 8) & 0FF00h)
	dw	((%2 >> 16) & 0FFFFh)
%endmacro

; Selector Local Descriptor Indicator
SA_LOCAL	equ		04h

; Selector Request Privilege Level
SA_RPL0		equ		0h
SA_RPL1		equ		1h
SA_RPL2		equ		2h
SA_RPL3		equ		3h

; Segment Attributes Composite
; Attribute Mapping
; | G | D/B | 0 | AVL | Segment Limit 2 | P | DPL | S | Type|

DA_PRESENT		equ	80h 	; Segment is present

; Descriptor Privilege Level
DA_DPL0			equ	0h  		; Descriptor Privilege is 0
DA_DPL1			equ	20h		; Descriptor Privilege is 1
DA_DPL2			equ	40h		; Descriptor Privilege is 2
DA_DPL3			equ	60h		; Descritpor Privilege is 3

; Segment Category
DA_CATEGORY_DC		equ	10h		; Segment is code segment or data segment
DA_CATEGORY_SG		equ	0h		; Segment is system segment or gate segment

; Code Or Data Segment Type
DA_TYPE_RO		equ	0h		; Read Only
DA_TYPE_RO_ACCESSED	equ	1h		; Read Only And Accessed
DA_TYPE_RW		equ	2h		; Read and Write
DA_TYPE_RW_ACCESSED	equ	3h		; Read Write And Accessed
DA_TYPE_RO_GTD		equ	4h		; Read Only, Grows towards down
DA_TYPE_RO_GTD_ACCESSED	equ	5h		; Read Only, Grows towards down, Accessed
DA_TYPE_RW_GTD		equ	6h		; Read Write, Grows towards down
DA_TYPE_RW_GTD_ACCESSED	equ	7h		; Read Write, Grows towards down, Accessed
DA_TYPE_EO		equ	8h		; Execute Only
DA_TYPE_EO_ACCESSED	equ	9h		; Execute Only, Accessed
DA_TYPE_ER		equ	0Ah		; Execute, Read
DA_TYPE_ER_ACCESSED	equ	0Bh		; Execute, Read, Accessed
DA_TYPE_EO_CF		equ	0Ch		; Execute Only, Conform Segment
DA_TYPE_EO_CF_ACCESSED	equ	0Dh		; Execute Only, Conform Segment, Accessed
DA_TYPE_ER_CF		equ	0Eh		; Execute, Read, Conform Segment
DA_TYPE_ER_CF_ACCESSED	equ	0Fh		; Execute, Read, Conform Segment, Accessed

; System Or Gate Segment Type
DA_TYPE_UNDEFINE0	equ	0h		; Undefined0
DA_TYPE_286TSS		equ	1h		; Available 286 TSS
DA_TYPE_LDT		equ	2h		; Local Descriptor Table
DA_TYPE_286TSS_BUSY	equ	3h		; Busy 286 TSS
DA_TYPE_286CALL_GATE	equ	4h		; 286 Call Gate
DA_TYPE_TASK_GATE	equ	5h		; Tast Gate
DA_TYPE_286INT_GATE	equ	6h		; 286 Interrupt Gate
DA_TYPE_286TRAP_GATE	equ	7h		; 286 Trap Gate
DA_TYPE_UNDEFINE1	equ	8h		; Undefined 1
DA_TYPE_386TSS		equ	9h		; Available 386 TSS
DA_TYPE_UNDEFINE2	equ	0Ah		; Undefined 2
DA_TYPE_386TSS_BUSY	equ	0Bh		; Busy 386 TSS
DA_TYPE_386CALL_GATE	equ	0Ch		; 386 Call Gate
DA_TYPE_UNDEFINE3	equ	0Dh		; Undefined 3
DA_TYPE_386INT_GATE	equ	0Eh		; 386 Interrupt Gate
DA_TYPE_386TRAP_GATE	equ	0Fh		; 386 Trap Gate

; Segment Granularity
DA_GRAN_BYTE		equ	0h		; Segment Granularity is byte
DA_GRAN_4KB		equ	8000h		; Segment Granularity is 4KB

; Code Segment D/B
DA_CODE_32BIT		equ	4000h		; Code Segment With Implicit 32 bit Operand
DA_CODE_16BIT		equ	0h		; Code Segment With Implicit 16 bit Operand

; Data Segment Towards Down D/B
DA_DATA_4G_UPPER_LIMIT	equ	4000h		; Data Segment With 4GB Upper Limit
DA_DATA_64K_UPPER_LIMIT	equ	0h		; Data Segment With 64KB Upper Limit

; Stack Segment D/B
DA_STACK_USE_ESP	equ	4000h		; Stack Segment, When using implicit instructions, use esp as stack pointer
DA_STACK_USE_SP		equ	0h		; Stack Segment, When using implicit instructions, use sp as stack pointer

; Common Composite

DA_LDT		equ		DA_PRESENT | DA_CATEGORY_SG | DA_TYPE_LDT

; TSS
DA_386TSS	equ		DA_PRESENT | DA_CATEGORY_SG | DA_TYPE_386TSS

; Gates
DA_386CALL_GATE		equ	DA_PRESENT | DA_CATEGORY_SG | DA_TYPE_386CALL_GATE
DA_386INT_GATE		equ	DA_PRESENT | DA_CATEGORY_SG | DA_TYPE_386INT_GATE

; Different Code Segments
DA_32BIT_EXE_ONLY_CODE		equ	DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_EO ; 32bit execute only code segment
DA_32BIT_EXE_READ_CODE		equ	DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_ER ; 32bit execute read code segment
DA_32BIT_EXE_CONFORM_CODE	equ	DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_EO_CF ; 32 bit execute only conform code segment
DA_32BIT_EXE_READ_CONFORM_CODE	equ	DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_ER_CF ; 32 bit execute read conform code segment

DA_16BIT_EXE_ONLY_CODE		equ	DA_CODE_16BIT | DA_PRESENT | DA_CATEGORY_DC| DA_TYPE_EO

; Different Data Segments
DA_READ_ONLY_DATA	equ	DA_CATEGORY_DC | DA_PRESENT | DA_TYPE_RO | DA_DATA_4G_UPPER_LIMIT ; read only data segment
DA_READ_WRITE_DATA	equ	DA_CATEGORY_DC | DA_PRESENT | DA_TYPE_RW | DA_DATA_4G_UPPER_LIMIT ; read write data segment
DA_READ_WRITE_ACCESSED_DATA	equ	DA_CATEGORY_DC | DA_PRESENT | DA_TYPE_RW_ACCESSED | DA_DATA_4G_UPPER_LIMIT ; read write accessed data segment

; Different Stack Segments
DA_32BIT_READ_WRITE_STACK	equ	DA_STACK_USE_ESP | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_RW ; read write stack segment
DA_32BIT_READ_WRITE_ACCESSED_STACK	equ	DA_STACK_USE_ESP | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_RW_ACCESSED ; read write accessed stack segment


;---------------------------------- Memory Paging Macros ---------------------------------------
PG_PRESENT		equ		1h
PG_READ_ONLY		equ		0h
PG_READ_WRITE		equ		2h
PG_US_SUPER		equ		0h
PG_US_USER		equ		4h
PG_PWT_WB		equ		0h	; Page Cache Strategy - Write Back
PG_PWT_WT		equ		8h	; Page Cache Strategy - Write Through
PG_PCD_ENABLE		equ		0h	; Page Cachable ? True
PG_PCD_DISABLE		equ		10h	; Page Cachable ? False
PG_ACCESSED		equ		20h	
PG_DIRTY		equ		40h	; Apply to page table entry only
PG_PS_4KB		equ		0h	; Apply to page directory entry only	
PG_PAT			equ		0h	; We don't use this, set to 0
PG_GLOBAL		equ		100h	;
