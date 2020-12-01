#ifndef	_LAVENDER_GLOBAL_H_
#define	_LAVENDER_GLOBAL_H_ 

#include	"protect.h"
#include	"process.h"
#include	"const.h"
#include	"type.h"
#include	"console.h"
#include	"tty.h"
#include	"drivers.h"

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
extern TASK		task_table[];
EXTERN TSS*		p_shared_tss;
extern irq_handler	irq_table[];
EXTERN int		ticks;
extern TTY		tty_table[];
extern CONSOLE		console_table[];	
EXTERN int		current_console;
extern DEV_DRIVER	dev_drv_map[];
extern u8*		fsbuf;
extern const int	FSBUF_SIZE; 
extern FILE_DESC	f_desc_table[];
extern INODE		inode_table[];
extern SUPER_BLOCK	super_block[];
EXTERN void*		root_inode;

#endif
