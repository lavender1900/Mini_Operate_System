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
		printf("<Ticks: %x>", get_ticks());
		milli_delay(350);
	}
}

PUBLIC	void	TestB()
{
	while(1) {
		printf("B.");
		milli_delay(400);
	}
}

PUBLIC	void	TestC()
{
	while(1) {
		printf("C.");
		milli_delay(500);
	}
}

PUBLIC	int	sys_get_ticks()
{
	return ticks;
}

PUBLIC	int	sys_write(char* buf, int len, PROCESS* p_proc)
{
	tty_write(&tty_table[p_proc->nr_tty], buf, len);
	return 0;
}
