#include	"process.h"
#include	"global.h"

PUBLIC	int	sys_get_ticks()
{
	return ticks;
}

PUBLIC	int	sys_write(char* buf, int len, PROCESS* p_proc)
{
	tty_write(&tty_table[p_proc->nr_tty], buf, len);
	return 0;
}
