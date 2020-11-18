#include	"const.h"
#include	"type.h"
#include	"proto.h"
#include	"global.h"
#include	"protect.h"
#include	"process.h"
#include	"i386macros.h"

void	init_descriptor(DESCRIPTOR* p, u32 base, u32 limit, u16 att);
u32	seg2phys(u16 seg);
void	restart();
void	donothing();

PRIVATE	void	restore_tss(PROCESS* p_proc);

PUBLIC	int kernel_main()
{
	disp_str("-------\"kernel_main\" begins-------\n");

	TASK*		p_task = 	task_table;
	PROCESS*	p_proc =	proc_table;
	u16		selector_ldt = 	SELECTOR_LDT;
	u16		selector_tss = 	SELECTOR_TSS;

	for (int i = 0; i < NR_TASKS; i++)
	{
		p_proc->pid = i;
		p_proc->ldt_selector = selector_ldt;
		p_proc->tss_selector = selector_tss;
		p_proc->restore_tss_func = (u32) restore_tss;

		// *************** Initialize Descriptors in LDT ******************
		kmemcpy(&gdt[SELECTOR_KERNEL_CS >> 3], &p_proc->ldts[1], sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 |= DA_DPL3;
		kmemcpy(&gdt[SELECTOR_KERNEL_DS >> 3], &p_proc->ldts[2], sizeof(DESCRIPTOR));
		p_proc->ldts[2].attr1 |= DA_DPL3; 
		
		// ************** Initialize LDT Descritpor in GDT ******************
		init_descriptor(&gdt[(selector_ldt & SA_FULL_MASK) >> 3],
				vir2phys(seg2phys(SELECTOR_KERNEL_DS), &p_proc->ldts),
				LDT_SIZE * sizeof(DESCRIPTOR) - 1,
				DA_LDT);

		// ****************** Initialize TSS **********************
		kmemset(&p_proc->tss, 0, sizeof(TSS));
		p_proc->tss.ss0 = SELECTOR_KERNEL_DS;
		p_proc->tss.esp0 = (u32)p_proc + (u32)sizeof(STACK_FRAME);

		init_descriptor(&gdt[(selector_tss & SA_FULL_MASK) >> 3],
			vir2phys(seg2phys(SELECTOR_KERNEL_DS), &p_proc->tss),
			sizeof(TSS) - 1,
			DA_386TSS);
		p_proc->tss.iobase = sizeof(TSS);

		p_proc->regs.cs = (SELECTOR_KERNEL_CS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
		p_proc->regs.ds = (SELECTOR_KERNEL_DS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
		p_proc->regs.es = (SELECTOR_KERNEL_DS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
		p_proc->regs.fs = (SELECTOR_KERNEL_DS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
		p_proc->regs.ss = (SELECTOR_KERNEL_DS & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
		p_proc->regs.gs = SELECTOR_VIDEO | SA_RPL3;
		p_proc->regs.eip = (u32) p_task->initial_eip;
		p_proc->regs.esp = ((u32) &p_proc->stack) + STACK_SIZE;

		p_proc->regs.eflags = 0x3202;

		p_proc++;
		p_task++;
		selector_ldt += 16;
		selector_tss += 16;
	}
	
	restart();
}

PRIVATE	void restore_tss(PROCESS* p_proc)
{
	DESCRIPTOR* p_desc = &gdt[(p_proc->tss_selector & SA_FULL_MASK) >> 3];
	p_desc->attr1++;
	p_desc->attr1--;
}
