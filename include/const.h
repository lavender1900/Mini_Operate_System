#ifndef	_LAVENDER_CONST_H_
#define	_LAVENDER_CONST_H_

#define	PUBLIC
#define	PRIVATE	static

#define	GDT_SIZE	128
#define	IDT_SIZE	256
#define	LDT_SIZE	3

#define	NR_TASKS	2

#define	EXTERN	extern

#define	INT_M_CTL	0x20 // Write to this port enable initialize 8259A master chip
#define	INT_M_CTLMASK	0x21 // Write to this port enable control interrupt response of master chip
#define	INT_S_CTL	0xA0 // Write to this port enable initialize 8259A slave chip
#define	INT_S_CTLMASK	0xA1 // Write to this port enable control interrupt response of slave chip

#define	PRIVILEGE_KERNEL	0
#define	PRIVILEGE_USER		3

#define	INT_VECTOR_IRQ0		0x20
#define	INT_VECTOR_IRQ8		0x28

#define	INT_VECTOR_DIVIDE	0
#define	INT_VECTOR_DEBUG	1
#define	INT_VECTOR_NMI		2
#define	INT_VECTOR_BREAKPOINT	3
#define	INT_VECTOR_OVERFLOW	4
#define	INT_VECTOR_BOUNDS	5
#define	INT_VECTOR_INVAL_OP	6
#define	INT_VECTOR_COPROC_NOT	7
#define	INT_VECTOR_DOUBLE_FAULT	8
#define	INT_VECTOR_COPROC_SEG	9
#define	INT_VECTOR_INVAL_TSS	10
#define	INT_VECTOR_SEG_NOT	11
#define	INT_VECTOR_STACK_FAULT	12
#define	INT_VECTOR_PROTECTION	13
#define	INT_VECTOR_PAGE_FAULT	14
#define	INT_VECTOR_COPROC_ERR	15

#endif
