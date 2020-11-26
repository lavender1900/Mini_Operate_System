#include	"type.h"
#include	"global.h"
#include	"protect.h"
#include	"ipc.h"

PUBLIC	u32 selector2base(u16 seg)
{
	DESCRIPTOR* p = &gdt[seg >> 3];
	return (p->base_high << 24 | p->base_mid << 16 | p->base_low);
}

PUBLIC	u32	ldt_proc_id2base(int proc_id)
{
	if (proc_id < 0 || proc_id >= NR_TASKS+NR_PROCS)
		panic("proc_id %d out of bound!", proc_id);

	PROCESS* p_proc = &proc_table[proc_id];
	DESCRIPTOR* p_desc = &p_proc->ldts[SELECTOR_KERNEL_DS >> 3];
	return (u32)(p_desc->base_high << 24) + (u32)(p_desc->base_mid << 16) + (u32)p_desc->base_low;
}
