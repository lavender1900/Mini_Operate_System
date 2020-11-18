#include	"type.h"
#include	"const.h"

#ifndef	_LAVENDER_PROCESS_H_
#define	_LAVENDER_PROCESS_H_

EXTERN	int	StackRing0Top;
EXTERN	int	StackRing3Top;

#define	STACK_SIZE	0x4000

typedef	void	(* task_func) ();

typedef	struct	s_stackframe
{
	u32	gs;
	u32	fs;
	u32	es;
	u32	ds;
	u32	edi;
	u32	esi;
	u32	ebp;
	u32	kernel_esp;
	u32	ebx;
	u32	edx;
	u32	ecx;
	u32	eax;
	u32	retaddr;
	u32	eip;
	u32	cs;
	u32	eflags;
	u32	esp;
	u32	ss;
} STACK_FRAME;

typedef	struct	s_proc
{
	STACK_FRAME regs;
	u16	ldt_selector;
	DESCRIPTOR	ldts[LDT_SIZE];
	u32	pid;
	char	p_name[16];
	char	stack[STACK_SIZE];
} PROCESS;

typedef	struct	s_task
{
	task_func	initial_eip;
	int		stack_size;
	char		name[32];
} TASK;	


#endif
