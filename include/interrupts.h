#ifndef	_LAVENDER_INTERRUPTS_H_
#define	_LAVENDER_INTERRUPTS_H_

#include	"type.h"

PUBLIC	void	divide_error();
PUBLIC	void	single_step_exception();
PUBLIC	void	nmi();
PUBLIC	void	breakpoint_exception();
PUBLIC	void	overflow();
PUBLIC	void	bounds_check();
PUBLIC	void	inval_opcode();
PUBLIC	void	copr_not_available();
PUBLIC	void	double_fault();
PUBLIC	void	copr_seg_overrun();
PUBLIC	void	inval_tss();
PUBLIC	void	segment_not_present();
PUBLIC	void	stack_exception();
PUBLIC	void	general_protection();
PUBLIC	void	page_fault();
PUBLIC	void	copr_error();

PUBLIC	void	add_irq_handler(int irq, irq_handler hander);


#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1	
#define CASCADE_IRQ	2
#define	AT_WINI_IRQ	14

#endif
