#ifndef	_LAVENDER_CLOCK_H_
#define	_LAVENDER_CLOCK_H_

#include	"const.h"

#define	TIMER_MODE		0x43
#define	TIMER0			0x40
#define	RATE_GENERATOR		0x34
#define	TIMER_FREQ		1193180
#define	TIME_INT_FREQ		100

PUBLIC	void	init_clock();	

#endif
