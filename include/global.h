#include	"protect.h"
#include	"process.h"
#include	"const.h"
#include	"type.h"

#ifndef	_LAVENDER_GLOBAL_H_
#define	_LAVENDER_GLOBAL_H_ 

#ifdef	GLOBAL_PARAMETER_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN int		cursor_pos;
EXTERN u8		gdt_ptr[6];
EXTERN DESCRIPTOR	gdt[GDT_SIZE];
EXTERN u8		idt_ptr[6];
EXTERN GATE		idt[IDT_SIZE];
EXTERN PROCESS		proc_table[NR_TASKS];
EXTERN PROCESS*		p_current_process;
EXTERN u32		PROCESS_TABLE_LDT_SELECTOR_OFFSET;
EXTERN u32		k_reenter;
EXTERN TASK		task_table[];
EXTERN TSS*		p_shared_tss;
EXTERN irq_handler	irq_table[];
EXTERN int		ticks;

#endif
