#include	"const.h"
#include	"type.h"
#include	"proto.h"
#include	"global.h"
#include	"process.h"

PUBLIC	void	clock_handler(int irq)
{
	ticks++;
	p_current_process->ticks--;
	
	if (k_reenter != 0)
	{
		return;
	}

	schedule();
}

PUBLIC	void	milli_delay(int milli_sec)
{
	int  t = get_ticks();
	
	while(((get_ticks() - t) * 1000 / TIME_INT_FREQ) < milli_sec)
		;
} 
