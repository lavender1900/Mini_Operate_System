#include	"type.h"
#include	"console.h"

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
PUBLIC void	disable_int();
PUBLIC void	enable_int();
PUBLIC void	TestA();
PUBLIC void	TestB();
PUBLIC void	TestC();

PUBLIC int	sys_get_ticks();
PUBLIC void	sys_call();
PUBLIC int	get_ticks();
PUBLIC void	milli_delay(int milli);
PUBLIC void	schedule();
PUBLIC void	init_keyboard();
PUBLIC void	init_clock();
PUBLIC void	add_irq_handler(int irq, irq_handler handler);

PUBLIC void	keyboard_read();
PUBLIC void	in_process();
PUBLIC void	task_tty();
PUBLIC int	is_current_console(CONSOLE* p_console);

PUBLIC void	clock_handler(int irq);
PUBLIC void	init_8259A();
PUBLIC void	spurious_irq(int irq);
PUBLIC void	init_prot();
PUBLIC void	enable_irq(int irq);
PUBLIC void	disable_irq(int irq);

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
