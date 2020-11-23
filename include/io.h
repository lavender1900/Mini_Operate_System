#ifndef	_LAVENDER_IO_H_
#define	_LAVENDER_IO_H_

#include	"const.h"

PUBLIC void* 	disp_str(char* src);
PUBLIC void*	disp_color_str(char* src, int color);
PUBLIC void	disp_int(int input);

PUBLIC	int	printf(const char* fmt, ...);

#endif
