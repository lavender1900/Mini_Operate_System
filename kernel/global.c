#define	GLOBAL_PARAMETER_HERE

#include	"type.h"
#include	"const.h"
#include	"protect.h"
#include	"global.h"
#include	"proto.h"

PUBLIC 	void	initializeGlobalParameters()
{
	cursor_pos = 0;
	PROCESS_TABLE_LDT_SELECTOR_OFFSET = (u32) (&((PROCESS*) 0)->ldt_selector);
	PROCESS_TABLE_ENTRY_OFFSET = sizeof(PROCESS);
	PROCESS_TABLE_TSS_SELECTOR_OFFSET = (u32) (&((PROCESS*) 0)->tss_selector);
	PROCESS_TABLE_TSS_OFFSET = (u32) (&((PROCESS*) 0)->tss);
}
