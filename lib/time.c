#include	"proto.h"
#include	"clock.h"

PUBLIC	void	milli_delay(int milli_sec)
{
	int  t = get_ticks();
	
	while(((get_ticks() - t) * 1000 / TIME_INT_FREQ) < milli_sec)
		;
} 
