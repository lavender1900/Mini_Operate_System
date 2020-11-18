#include	"type.h"
#include	"const.h"
#include	"global.h"
#include	"proto.h"
#include	"process.h"

void	donothing();

PUBLIC	void	schedule()
{
	PROCESS* p;
	int	greatest_ticks = 0;
	
	while (!greatest_ticks)
	{
		for (p = proc_table; p < proc_table + NR_TASKS; p++)
		{
			if (p->ticks > greatest_ticks)
			{
				greatest_ticks = p->ticks;
				p_current_process = p;
			}
		}


		if (!greatest_ticks)
		{
			for (p = proc_table; p < proc_table + NR_TASKS; p++)
			{
				p->ticks = p->priority;
			}
		}
	}
}

PUBLIC	void	TestA()
{
	while(1) {
		disp_str("A.");
		milli_delay(150);
	}
}

PUBLIC	void	TestB()
{
	while(1) {
		disp_str("B.");
		milli_delay(200);
	}
}

PUBLIC	void	TestC()
{
	while(1) {
		disp_str("C.");
		milli_delay(200);
	}
}

PUBLIC	int	sys_get_ticks()
{
	return ticks;
}
