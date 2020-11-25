#ifndef	_LAVENDER_IO_H_
#define	_LAVENDER_IO_H_

#include	"const.h"
#include	"type.h"

PUBLIC void* 	disp_str(char* src);
PUBLIC void*	disp_color_str(char* src, int color);
PUBLIC void	disp_int(int input);

PUBLIC	int	printf(const char* fmt, ...);
PUBLIC	int	vsprintf(char* buf, const char* fmt, va_list args);

#endif
