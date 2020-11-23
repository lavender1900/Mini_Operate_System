#ifndef	_LAVENDER_SYSTEM_CALLS_H_
#define	_LAVENDER_SYSTEM_CALLS_H_

#include	"process.h"

PUBLIC	int	sys_get_ticks();
PUBLIC	int	sys_write(char* buf, int len, PROCESS* p_proc);

#endif
