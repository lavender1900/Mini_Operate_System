#ifndef	_LAVENDER_CONSOLE_H_
#define	_LAVENDER_CONSOLE_H_

#include	"type.h"

typedef	struct	s_console
{
	unsigned int	current_start_addr;
	unsigned int	original_addr;
	unsigned int	v_mem_limit;
	unsigned int	cursor;
} CONSOLE;

#define	SCR_UP		0
#define	SCR_DOWN	1

#define	SCREEN_SIZE	80 * 25
#define	SCREEN_WIDTH	80

#endif
