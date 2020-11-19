#ifndef	_LAVENDER_CONST_H_
#define	_LAVENDER_CONST_H_

#define	PUBLIC
#define	PRIVATE	static

#define	GDT_SIZE	128
#define	IDT_SIZE	256
#define	LDT_SIZE	3

#define	NR_TASKS	4
#define	NR_IRQS		16
#define	NR_CONSOLES	3
#define	NR_SYS_CALL	1

#define	EXTERN	extern

#define	INT_M_CTL	0x20 // Write to this port enable initialize 8259A master chip
#define	INT_M_CTLMASK	0x21 // Write to this port enable control interrupt response of master chip
#define	INT_S_CTL	0xA0 // Write to this port enable initialize 8259A slave chip
#define	INT_S_CTLMASK	0xA1 // Write to this port enable control interrupt response of slave chip

#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1	

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

#define	INT_VECTOR_SYS_CALL	0x70

#define	TIMER_MODE		0x43
#define	TIMER0			0x40
#define	RATE_GENERATOR		0x34
#define	TIMER_FREQ		1193180
#define	TIME_INT_FREQ		100

#define	CRTC_ADDR_REG		0x3D4
#define	CRTC_DATA_REG		0x3D5
#define	START_ADDR_H		0xC
#define	START_ADDR_L		0xD
#define	CURSOR_H		0xE
#define	CURSOR_L		0xF
#define	V_MEM_BASE		0xB8000
#define	V_MEM_SIZE		0x8000
#define	DEFAULT_CHAR_COLOR	0x0F

#endif
