#ifndef	_LAVENDER_CONSOLE_H_
#define	_LAVENDER_CONSOLE_H_

#include	"type.h"
#include	"tty.h"

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

#define	CRTC_ADDR_REG		0x3D4
#define	CRTC_DATA_REG		0x3D5
#define	START_ADDR_H		0xC
#define	START_ADDR_L		0xD
#define	CURSOR_H		0xE
#define	CURSOR_L		0xF
#define	V_MEM_BASE		0xB8000
#define	V_MEM_SIZE		0x8000
#define	DEFAULT_CHAR_COLOR	0x0F
#define	RED_CHAR		0x0C
#define	GRAY_CHAR		0x08

PUBLIC	int	is_current_console(CONSOLE* p_console);
PUBLIC	void	select_console(int nr_con);
PUBLIC	void	scroll_screen(CONSOLE* p, int direction);
PUBLIC	void	init_screen(TTY* p_tty);
PUBLIC	void	out_char(CONSOLE* p_con, char ch);

#endif
