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
#define	NR_SYS_CALL	2

#define	EXTERN	extern


#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1	



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
