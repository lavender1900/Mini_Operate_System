#ifndef	_LAVENDER_TTY_H_
#define	_LAVENDER_TTY_H_ 

#include	"const.h"
#include	"type.h"

#define	TTY_IN_BYTES	256

struct	s_console;

typedef	struct	s_tty
{
	u32	in_buf[TTY_IN_BYTES];
	u32*	p_in_buf_head;
	u32*	p_in_buf_tail;
	int	in_buf_count;
	
	struct	s_console*	p_console;
} TTY;

#endif
