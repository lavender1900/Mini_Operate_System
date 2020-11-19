#include	"type.h"
#include	"const.h"
#include	"proto.h"
#include	"console.h"
#include	"global.h"

PUBLIC	int	is_current_console(CONSOLE* p_console)
{
	return (p_console == &console_table[current_console]);
}

PRIVATE	void	set_cursor(u32 pos)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (pos >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, pos & 0xFF);
	enable_int();
}
	

PUBLIC	void	out_char(CONSOLE* p_console, char ch)
{
	u8* p_vmem = (u8*) (V_MEM_BASE + cursor_pos);
	*p_vmem++ = ch;
	*p_vmem++ = DEFAULT_CHAR_COLOR;
	cursor_pos += 2;

	set_cursor(cursor_pos);
}
