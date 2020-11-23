#define	GLOBAL_PARAMETER_HERE

#include	"const.h"
#include	"type.h"
#include	"global.h"
#include	"process.h"
#include	"system_calls.h"
#include	"tty.h"
#include	"console.h"

PUBLIC	TASK	task_table[NR_TASKS] = {{TestA, STACK_SIZE, "Process A"}, {TestB, STACK_SIZE, "Process B"},
					{TestC, STACK_SIZE, "Process C"}, {task_tty, STACK_SIZE, "TTY"}};
PUBLIC	TTY		tty_table[NR_CONSOLES];
PUBLIC	CONSOLE		console_table[NR_CONSOLES];
PUBLIC	TSS		shared_tss;
PUBLIC	irq_handler	irq_table[NR_IRQS];
PUBLIC	system_call	sys_call_table[NR_SYS_CALL] = {sys_get_ticks, sys_write};

PUBLIC 	void	initializeGlobalParameters()
{
	cursor_pos = 0;
	ticks = 0;
	k_reenter = 0;
	p_shared_tss = &shared_tss;
	p_current_process = &proc_table[0];
	PROCESS_TABLE_LDT_SELECTOR_OFFSET = (u32) (&((PROCESS*) 0)->ldt_selector);
}
