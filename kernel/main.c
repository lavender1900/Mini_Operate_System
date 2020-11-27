#include	"type.h"
#include	"global.h"
#include	"protect.h"
#include	"process.h"
#include	"i386macros.h"
#include	"asm_lib.h"
#include	"clock.h"
#include	"keyboard.h"
#include	"tty.h"
#include	"io.h"
#include	"page.h"
#include	"ipc.h"
#include	"string.h"

void	process_start();
PRIVATE void	restore_tss_func(DESCRIPTOR* p);

PUBLIC	int kernel_main()
{
	disp_str("-------\"kernel_main\" begins-------\n");
	
	// init 8259A interrupt handler mappings
	init_clock();
	init_keyboard();

	// init tty and console
	init_tty();
	

	TASK*		p_task = 	task_table;
	PROCESS*	p_proc =	proc_table;
	u16		selector_ldt = 	SELECTOR_LDT;

	u8		dpl = DA_DPL3;
	u16		cs = (SELECTOR_KERNEL_CS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	u16		ss = (SELECTOR_KERNEL_DS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	u32		eflags = 0x202;
	int		proc_type = PROC_TYPE_USER;
	
	int priority = 200;

	for (int i = 0; i < NR_PROCS+NR_TASKS; i++)
	{

		if (i == NR_PROCS)
		{
			dpl = DA_DPL1;
			cs = (SELECTOR_KERNEL_CS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL1;	
			ss = (SELECTOR_KERNEL_DS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL1;
			eflags = 0x1202;
			proc_type = PROC_TYPE_TASK;
			priority = 600;
		}

		p_proc->pid = i;
		p_proc->ldt_selector = selector_ldt;
		p_proc->priority = p_proc->ticks = priority;
		p_proc->nr_tty = i/2;
		p_proc->proc_type = proc_type;
		p_proc->p_flags = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;
		_strcpy(p_proc->name, p_task->name);

		// *************** Initialize Descriptors in LDT ******************
		kmemcpy(&gdt[SELECTOR_KERNEL_CS >> 3], &p_proc->ldts[1], sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 |= dpl;
		kmemcpy(&gdt[SELECTOR_KERNEL_DS >> 3], &p_proc->ldts[2], sizeof(DESCRIPTOR));
		p_proc->ldts[2].attr1 |= dpl; 
		
		// ************** Initialize LDT Descritpor in GDT ******************
		init_descriptor(&gdt[(selector_ldt & SA_FULL_MASK) >> 3],
				vir2linear(selector2base(SELECTOR_KERNEL_DS), &p_proc->ldts),
				LDT_SIZE * sizeof(DESCRIPTOR) - 1,
				DA_LDT);

		// ****************** Initialize TSS **********************
		kmemset(p_shared_tss, 0, sizeof(TSS));
		p_shared_tss->ss0 = SELECTOR_KERNEL_DS;
		p_shared_tss->esp0 = (u32)p_proc + (u32)sizeof(STACK_FRAME);

		init_descriptor(&gdt[(SELECTOR_TSS & SA_FULL_MASK) >> 3],
			vir2linear(selector2base(SELECTOR_KERNEL_DS), p_shared_tss),
			sizeof(TSS) - 1,
			DA_386TSS);
		p_shared_tss->iobase = sizeof(TSS);

		restore_tss_func(&gdt[SELECTOR_TSS >> 3]);

		p_proc->regs.cs = cs;
		p_proc->regs.ds = ss; 
		p_proc->regs.es = ss; 
		p_proc->regs.fs = ss; 
		p_proc->regs.ss = ss; 
		p_proc->regs.gs = SELECTOR_VIDEO | SA_RPL3;
		p_proc->regs.eip = (u32) p_task->initial_eip;
		p_proc->regs.esp = ((u32) &p_proc->stack) + STACK_SIZE;

		p_proc->regs.eflags = eflags;

		p_proc++;
		p_task++;
		priority -= 50;
		selector_ldt += 8;
	}
	
	process_start();
}

PRIVATE void restore_tss_func(DESCRIPTOR* p_desc)
{
	p_desc->attr1 = DA_386TSS;
}
