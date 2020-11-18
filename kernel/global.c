#define	GLOBAL_PARAMETER_HERE

#include	"type.h"
#include	"const.h"
#include	"protect.h"
#include	"global.h"
#include	"proto.h"

PUBLIC	TASK	task_table[NR_TASKS] = {{TestA, STACK_SIZE, "Process A"}, {TestB, STACK_SIZE, "Process B"}};
PUBLIC	TSS	shared_tss;

PUBLIC 	void	initializeGlobalParameters()
{
	cursor_pos = 0;
	k_reenter = -1;
	p_shared_tss = &shared_tss;
	p_current_process = &proc_table[0];
	PROCESS_TABLE_LDT_SELECTOR_OFFSET = (u32) (&((PROCESS*) 0)->ldt_selector);
}
