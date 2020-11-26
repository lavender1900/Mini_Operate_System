#ifndef	_LAVENDER_CONST_H_
#define	_LAVENDER_CONST_H_

#define	PUBLIC
#define	PRIVATE	static

#define	GDT_SIZE	128
#define	IDT_SIZE	256
#define	LDT_SIZE	3

#define	NR_PROCS	3
#define	NR_TASKS	2
#define	NR_IRQS		16
#define	NR_CONSOLES	3
#define	NR_SYS_CALL	4

#define	EXTERN	extern

#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1	

#endif
