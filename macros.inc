%macro	Descriptor	3 ; Three parameters
	dw	%2 & 0FFFFh ; Segment Limit 1
	dw	%1 & 0FFFFh ; Segment Base Addr 1 (first two bytes)
	db	(%1 >> 16) & 0FFh ; Segment Base Addr 1 (third byte)
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh) ; Attributes + Segment Limit 2
	db	(%1 >> 24) & 0FFh ; Segment Base Addr 2 (forth byte)
%endmacro

; Segment Attributes Composite
; Attribute Mapping
; | G | D/B | 0 | AVL | Segment Limit 2 | P | DPL | S | Type|

DA_PRESENT		equ	0080h 	; Segment is present

; Descriptor Privilege Level
DA_DPL0			equ	0h  		; Descriptor Privilege is 0
DA_DPL1			equ	0020h		; Descriptor Privilege is 1
DA_DPL2			equ	0040h		; Descriptor Privilege is 2
DA_DPL3			equ	0060h		; Descritpor Privilege is 3

; Segment Category
DA_CATEGORY_DC		equ	0010h		; Segment is code segment or data segment
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
DA_TYPE_ER		equ	10h		; Execute, Read
DA_TYPE_ER_ACCESSED	equ	11h		; Execute, Read, Accessed
DA_TYPE_EO_CF		equ	12h		; Execute Only, Conform Segment
DA_TYPE_EO_CF_ACCESSED	equ	13h		; Execute Only, Conform Segment, Accessed
DA_TYPE_ER_CF		equ	14h		; Execute, Read, Conform Segment
DA_TYPE_ER_CF_ACCESSED	equ	15h		; Execute, Read, Conform Segment, Accessed

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
DA_TYPE_UNDEFINE2	equ	10h		; Undefined 2
DA_TYPE_386TSS_BUSY	equ	11h		; Busy 386 TSS
DA_TYPE_386CALL_GATE	equ	12h		; 386 Call Gate
DA_TYPE_UNDEFINE3	equ	13h		; Undefined 3
DA_TYPE_386INT_GATE	equ	14h		; 386 Interrupt Gate
DA_TYPE_386TRAP_GATE	equ	15h		; 386 Trap Gate

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

; Different Code Segments
DA_32BIT_EXE_ONLY_CODE		equ	DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_EO ; 32bit execute only code segment
DA_32BIT_EXE_READ_CODE		equ	DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_ER ; 32bit execute read code segment
DA_32BIT_EXE_CONFORM_CODE	equ	DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_EO_CF ; 32 bit execute only conform code segment
DA_32BIT_EXE_READ_CONFORM_CODE	equ	DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_ER_CF ; 32 bit execute read conform code segment

; Different Data Segments
DA_READ_ONLY_DATA	equ	DA_CATEGORY_DC | DA_PRESENT | DA_TYPE_RO ; read only data segment
DA_READ_WRITE_DATA	equ	DA_CATEGORY_DC | DA_PRESENT | DA_TYPE_RW ; read write data segment
DA_READ_WRITE_ACCESSED_DATA	equ	DA_CATEGORY_DC | DA_PRESENT | DA_TYPE_RW_ACCESSED ; read write accessed data segment

; Different Stack Segments
DA_32BIT_READ_WRITE_STACK	equ	DA_STACK_USE_ESP | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_RW ; read write stack segment
DA_32BIT_READ_WRITE_ACCESSED_STACK	equ	DA_STACK_USE_ESP | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_RW_ACCESSED ; read write accessed stack segment
