#ifndef	_LAVENDER_SYSTEM_CALLS_H_
#define	_LAVENDER_SYSTEM_CALLS_H_

#include	"process.h"
#include	"ipc.h"

PUBLIC	int	sys_get_ticks();
PUBLIC	int	sys_write(char* buf, int len, PROCESS* p_proc);
PUBLIC	int	sys_sendrec(int func, int src_dest, PROCESS* p_proc, MESSAGE* msg);
PUBLIC	int	sys_sendint(int func, int dest, PROCESS* p_proc, MESSAGE* msg);
PUBLIC	int	sys_printx(int unused, char* buf, PROCESS* p_proc);

#endif
