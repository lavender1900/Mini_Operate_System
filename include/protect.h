#ifndef _LAVENDER_PROTECT_H_
#define	_LAVENDER_PROTECT_H_

#include	"type.h"
#include	"const.h"

typedef struct s_descriptor
{
	u16	limit_low;
	u16	base_low;
	u8	base_mid;
	u8	attr1;		// P DPL S Type
	u8	limit_high_attr2; // G D/B 0 AVL limit_high
	u8	base_high;
} DESCRIPTOR;

typedef	struct s_gate
{
	u16	offset_low;
	u16	selector;
	u8	dcount;
	u8	attr;
	u16	offset_high;
} GATE;

typedef	struct	s_tss
{
	u32	backlink;
	u32	esp0;
	u32	ss0;
	u32	esp1;
	u32	ss1;
	u32	esp2;
	u32	ss2;
	u32	cr3;
	u32	eip;
	u32	flags;
	u32	eax;
	u32	ecx;
	u32	edx;
	u32	ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u32	es;
	u32	cs;
	u32	ss;
	u32	ds;
	u32	fs;
	u32	gs;
	u32	ldt;
	u16	trap;
	u16	iobase;
} TSS;

#define	SELECTOR_FLAT_C		0x8
#define	SELECTOR_FLAT_RW	0x10
#define	SELECTOR_VIDEO		(0x18 | 0x3)
#define	SELECTOR_TSS		0x20
#define	SELECTOR_LDT		0x28

#define	SELECTOR_KERNEL_CS	SELECTOR_FLAT_C	
#define	SELECTOR_KERNEL_DS	SELECTOR_FLAT_RW	
#define	SELECTOR_KERNEL_GS	SELECTOR_VIDEO	

#define	PRIVILEGE_KERNEL	0
#define	PRIVILEGE_TASK		1
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

#define	SA_RPL_MASK		0xFFFC
#define	SA_TI_MASK		0xFFFB
#define	SA_FULL_MASK		0xFFF8

#define	vir2phys(seg_base, vir) (u32)((u32)seg_base + (u32)vir)

PUBLIC	void	cpu_reserved_exception_handler(int vec_no, int errcode, int eip, int cs, int eflags);
PUBLIC	void	init_descriptor(DESCRIPTOR* p_desc, u32 base, u32 limit, u16 attr);
PUBLIC	u32	seg2phys(u16 seg);
PUBLIC	void	init_protect_mode_interrupt_mechanism();

#endif
