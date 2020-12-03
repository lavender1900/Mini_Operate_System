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
#define	NR_SYS_CALL	5

// Device major number
#define	NO_DEV		0
#define	DEV_FLOPPY	1
#define	DEV_CDROM	2
#define	DEV_HD		3
#define	DEV_CHAR_TTY	4
#define	DEV_SCSI	5

// make device number from major and minor number
#define	MAJOR_SHIFT	8
#define	MAKE_DEV(a, b)	((a << MAJOR_SHIFT) | (b & 0xFF))

#define	MAJOR(x)	((x >> MAJOR_SHIFT) & 0xFF)
#define	MINOR(x)	(x & 0xFF)

#define	EXTERN	extern

#endif
