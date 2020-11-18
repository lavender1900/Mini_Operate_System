#include	"const.h"
#include	"type.h"
#include	"proto.h"
#include	"global.h"
#include	"process.h"

PUBLIC	void	clock_handler(int irq)
{
	disp_str("#");
	
	if (k_reenter != 0)
	{
		disp_str("!!");
		return;
	}

	p_current_process++;
	if (p_current_process >= proc_table + NR_TASKS)
		p_current_process = proc_table;
}
