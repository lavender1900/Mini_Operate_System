#ifndef	_LAVENDER_PROCESS_H_
#define	_LAVENDER_PROCESS_H_

#include	"type.h"
#include	"const.h"
#include	"protect.h"
#include	"fs.h"

EXTERN	int	StackRing0Top;
EXTERN	int	StackRing3Top;

#define	STACK_SIZE	0x4000
#define	PROC_TYPE_TASK	0
#define PROC_TYPE_USER	1

#define	proc2pid(x)	((PROCESS*)x - (PROCESS*)proc_table)

typedef	void	(* task_func) ();

/**
 * MESSAGE mechanism is borrowed from MINIX
 */
struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	u64	m3l1;
	u64	m3l2;
	void*	m3p1;
	void*	m3p2;
};
typedef struct {
	int source;
	int type;
	int RETVAL;
	int DEVICE;
	int CNT;
	int PROC_NR;
	int REQUEST;
	int FLAGS;
	int NAME_LEN;
	int FD;
	void* PATHNAME;
	u64 POSITION;
	void* BUF;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} MESSAGE;

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

	int	a;
	int	b;
	int	c;
	int	priority;
	int	d;
	int	e;
	int	f;
	int	g;
	int	ticks;
	u32	pid;
	u32	nr_tty;
	int	proc_type;
	char	name[16];
	int	p_flags;
	MESSAGE* p_msg;
	int	p_recvfrom;
	int	p_sendto;
	int	has_int_msg;
	struct s_proc* q_sending;
	struct s_proc* next_sending;
	FILE_DESC*	filp[NR_FILES];
	char	stack[STACK_SIZE];
} PROCESS;

typedef	struct	s_task
{
	task_func	initial_eip;
	int		stack_size;
	char		name[32];
} TASK;	

PUBLIC	void	TestA();
PUBLIC	void	TestB();
PUBLIC	void	TestC();

#endif
