#include	"type.h"

#ifndef _LAVENDER_PROTO_H_
#define	_LAVENDER_PROTO_H_

PUBLIC void*	 kmemcpy(void* src, void* dest, int size);
PUBLIC void*	kmemset(void* dest, u8 value, int setBytes);
PUBLIC void* 	disp_str(void* src);
PUBLIC void*	disp_color_str(void* src, int color);
PUBLIC void	disp_int(int input);
PUBLIC char*	itoa(char* str, int num);
PUBLIC void	 out_byte(u32 port, u32 value); 
PUBLIC u32	in_byte(u32 port);
PUBLIC void	delay(int time);
PUBLIC void	TestA();
PUBLIC void	TestB();

PUBLIC void	clock_handler(int irq);
PUBLIC void	init_8259A();
PUBLIC void	spurious_irq(int irq);
PUBLIC void	init_prot();

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

#endif
