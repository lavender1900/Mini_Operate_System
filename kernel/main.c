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
void	testA();

PUBLIC	int kernel_main()
{
	disp_str("-------\"kernel_main\" begins-------\n");

	// Start a brand new Process running on ring 3

	PROCESS* p_proc = &proc_table[0];
	p_proc->ldt_selector = SELECTOR_LDT;
	p_proc->tss_selector = SELECTOR_TSS;

	// ****************** Initialize LDT Descriptor in GDT ******************
	init_descriptor(&gdt[(SELECTOR_LDT & SA_FULL_MASK) >> 3],
			vir2phys(seg2phys(SELECTOR_KERNEL_DS), &p_proc->ldts),
			LDT_SIZE * sizeof(DESCRIPTOR) - 1,
			DA_LDT);

	// ****************** Initialize TSS **********************
	kmemset(&p_proc->tss, 0, sizeof(TSS));
	p_proc->tss.ss0 = SELECTOR_KERNEL_DS;
	p_proc->tss.esp0 = StackRing0Top;
	init_descriptor(&gdt[(SELECTOR_TSS & SA_FULL_MASK) >> 3],
			vir2phys(seg2phys(SELECTOR_KERNEL_DS), &p_proc->tss),
			sizeof(TSS) - 1,
			DA_386TSS);
	p_proc->tss.iobase = sizeof(TSS);

	// *************** Initialize Descriptors in LDT ******************
	kmemcpy(&gdt[SELECTOR_KERNEL_CS >> 3], &p_proc->ldts[0], sizeof(DESCRIPTOR));
	p_proc->ldts[0].attr1 |= (0x3 << 5);
	kmemcpy(&gdt[SELECTOR_KERNEL_DS >> 3], &p_proc->ldts[1], sizeof(DESCRIPTOR));
	p_proc->ldts[1].attr1 |= (0x3 << 5);

	p_proc->regs.cs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	p_proc->regs.ds = (0x10 & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	p_proc->regs.es = (0x10 & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	p_proc->regs.fs = (0x10 & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	p_proc->regs.ss = (0x10 & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	p_proc->regs.gs = (0x10 & SA_RPL_MASK & SA_TI_MASK) | SA_LOCAL | SA_RPL3;
	p_proc->regs.eip = (u32) testA;
	p_proc->regs.esp = (u32) StackRing3Top;
	p_proc->regs.eflags = 0x1202;

	restart();
}

void	testA()
{
	int i = 0;
	while(1) {
		disp_str("A");
		disp_int(i++);
		disp_str(".");
		delay(1);
	}
}
