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

#endif
