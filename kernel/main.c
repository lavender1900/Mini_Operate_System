#include	"const.h"
#include	"type.h"
#include	"proto.h"
#include	"global.h"
#include	"protect.h"
#include	"process.h"
#include	"i386macros.h"

void	init_descriptor(DESCRIPTOR* p, u32 base, u32 limit, u16 att);
u32	seg2phys(u16 seg);
void	process_start();
void	donothing();
PRIVATE void	restore_tss_func(DESCRIPTOR* p);

PUBLIC	int kernel_main()
{
	disp_str("-------\"kernel_main\" begins-------\n");
	
	// init 8259A interrupt handler mappings
	init_clock();
	init_keyboard();
	
	// Change the frequency of time interrupt to 100HZ
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (u8) (TIMER_FREQ / TIME_INT_FREQ));
	out_byte(TIMER0, (u8) ((TIMER_FREQ / TIME_INT_FREQ) >> 8));

	TASK*		p_task = 	task_table;
	PROCESS*	p_proc =	proc_table;
	u16		selector_ldt = 	SELECTOR_LDT;

	u8		dpl = DA_DPL3;
	u16		cs = (SELECTOR_KERNEL_CS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	u16		ss = (SELECTOR_KERNEL_DS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	
	int priority = 200;

	for (int i = 0; i < NR_TASKS; i++)
	{

		if (i == NR_TASKS - 1)
		{
			dpl = DA_DPL1;
			cs = (SELECTOR_KERNEL_CS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL1;	
			ss = (SELECTOR_KERNEL_DS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL1;
		}

		p_proc->pid = i;
		p_proc->ldt_selector = selector_ldt;
		p_proc->priority = p_proc->ticks = priority;

		// *************** Initialize Descriptors in LDT ******************
		kmemcpy(&gdt[SELECTOR_KERNEL_CS >> 3], &p_proc->ldts[1], sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 |= dpl;
		kmemcpy(&gdt[SELECTOR_KERNEL_DS >> 3], &p_proc->ldts[2], sizeof(DESCRIPTOR));
		p_proc->ldts[2].attr1 |= dpl; 
		
		// ************** Initialize LDT Descritpor in GDT ******************
		init_descriptor(&gdt[(selector_ldt & SA_FULL_MASK) >> 3],
				vir2phys(seg2phys(SELECTOR_KERNEL_DS), &p_proc->ldts),
				LDT_SIZE * sizeof(DESCRIPTOR) - 1,
				DA_LDT);

		// ****************** Initialize TSS **********************
		kmemset(p_shared_tss, 0, sizeof(TSS));
		p_shared_tss->ss0 = SELECTOR_KERNEL_DS;
		p_shared_tss->esp0 = (u32)p_proc + (u32)sizeof(STACK_FRAME);

		init_descriptor(&gdt[(SELECTOR_TSS & SA_FULL_MASK) >> 3],
			vir2phys(seg2phys(SELECTOR_KERNEL_DS), p_shared_tss),
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

		p_proc->regs.eflags = 0x3202;

		p_proc++;
		p_task++;
		priority += 100;
		selector_ldt += 8;
	}
	
	process_start();
}

PRIVATE void restore_tss_func(DESCRIPTOR* p_desc)
{
	p_desc->attr1 = DA_386TSS;
}
