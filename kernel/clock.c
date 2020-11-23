#include	"clock.h"
#include	"global.h"
#include	"asm_lib.h"
#include	"interrupts.h"

PRIVATE	void	clock_handler(int irq);
PRIVATE	void	change_time_interrupt_freq();
PRIVATE	void	schedule();

PUBLIC	void	init_clock()
{
	change_time_interrupt_freq();
	add_irq_handler(CLOCK_IRQ, clock_handler);
	enable_irq(CLOCK_IRQ);
}

PRIVATE	void	clock_handler(int irq)
{
	if (k_reenter != 0)
	{
		return;
	}

	ticks++;
	p_current_process->ticks--;

	schedule();
}

PRIVATE	void	change_time_interrupt_freq()
{
	// Change the frequency of time interrupt to 100HZ
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (u8) (TIMER_FREQ / TIME_INT_FREQ));
	out_byte(TIMER0, (u8) ((TIMER_FREQ / TIME_INT_FREQ) >> 8));
}

PRIVATE	void	schedule()
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
