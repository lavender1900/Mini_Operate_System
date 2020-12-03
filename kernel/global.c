#define	GLOBAL_PARAMETER_HERE

#include	"const.h"
#include	"type.h"
#include	"global.h"
#include	"process.h"
#include	"system_calls.h"
#include	"tty.h"
#include	"console.h"
#include	"proto.h"
#include	"hd.h"
#include	"drivers.h"
#include	"fs.h"

PUBLIC	TASK	task_table[NR_TASKS+NR_PROCS] = {{TestA, STACK_SIZE, "Process A"}, {TestB, STACK_SIZE, "Process B"},
					{TestC, STACK_SIZE, "Process C"}, {task_tty, STACK_SIZE, "TTY"}, 
					{task_sys, STACK_SIZE, "syscall"}/*, {task_hd, STACK_SIZE, "hard disk"},
					{task_fs, STACK_SIZE, "file system"}*/};
PUBLIC	TTY		tty_table[NR_CONSOLES];
PUBLIC	CONSOLE		console_table[NR_CONSOLES];
PUBLIC	TSS		shared_tss;
PUBLIC	irq_handler	irq_table[NR_IRQS];
PUBLIC	system_call	sys_call_table[NR_SYS_CALL] = {sys_get_ticks, sys_write, sys_sendrec, sys_sendint, sys_printx};
PUBLIC	DEV_DRIVER	dev_drv_map[] = {{INVALID_DRIVER}, {INVALID_DRIVER}, {INVALID_DRIVER},
					{TASK_HD_DRIVER}, {TASK_TTY_DRIVER}, {INVALID_DRIVER}};	
PUBLIC	u8*		fsbuf = (u8*)0x600000;
PUBLIC	const	int	FSBUF_SIZE = 0x100000;
PUBLIC	FILE_DESC	f_desc_table[NR_FILE_DESC];
PUBLIC	INODE		inode_table[NR_INODE];
PUBLIC	SUPER_BLOCK	super_block[NR_SUPER_BLOCK];

PUBLIC 	void	initializeGlobalParameters()
{
	cursor_pos = 0;
	ticks = 0;
	k_reenter = 0;
	p_shared_tss = &shared_tss;
	p_current_process = &proc_table[0];
	PROCESS_TABLE_LDT_SELECTOR_OFFSET = (u32) (&((PROCESS*) 0)->ldt_selector);
}
