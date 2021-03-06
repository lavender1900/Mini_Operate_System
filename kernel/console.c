#include	"type.h"
#include	"const.h"
#include	"proto.h"
#include	"console.h"
#include	"global.h"
#include	"asm_lib.h"

PRIVATE	void	set_video_start_addr(u32 addr);
PRIVATE	void	set_cursor(u32 pos);
PRIVATE	void	flush(CONSOLE* p);

PUBLIC	int	is_current_console(CONSOLE* p_console)
{
	return (p_console == &console_table[current_console]);
}

PUBLIC	void	out_char(CONSOLE* p_console, char ch)
{
	u8* p_vmem = (u8*) (V_MEM_BASE + p_console->cursor * 2); 

	switch (ch) {
	case '\n':
		if (p_console->cursor < p_console->original_addr + p_console->v_mem_limit - SCREEN_WIDTH) {
			p_console->cursor = p_console->original_addr + SCREEN_WIDTH * (1 + ((p_console->cursor - p_console->original_addr) / SCREEN_WIDTH));
		} 
		break;
	case '\b':
		if (p_console->cursor > p_console->original_addr) {
			p_console->cursor--;
			*(p_vmem - 2) = ' ';
			*(p_vmem - 1) = DEFAULT_CHAR_COLOR;
		}
		break;
	default:
		if (p_console->cursor < p_console->original_addr + p_console->v_mem_limit - 1) {
			*p_vmem++ = ch;
			*p_vmem++ = DEFAULT_CHAR_COLOR;
			p_console->cursor++;
		}
		break;
	}

	while(p_console->cursor >= p_console->current_start_addr + SCREEN_SIZE) {
		scroll_screen(p_console, SCR_DOWN);
	}

	flush(p_console);
}

PUBLIC	void	init_screen(TTY* p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;

	int v_mem_size = V_MEM_SIZE >> 1;
	
	int con_v_mem_size = v_mem_size / NR_CONSOLES;

	p_tty->p_console->original_addr = nr_tty * con_v_mem_size;	
	p_tty->p_console->v_mem_limit = con_v_mem_size;
	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;

	p_tty->p_console->cursor = p_tty->p_console->original_addr;

	if (nr_tty == 0) {
		p_tty->p_console->cursor = cursor_pos / 2;
		cursor_pos = 0;
	} else {
	//	out_char(p_tty->p_console, nr_tty + '0');
//		out_char(p_tty->p_console, '#');
	}

	set_cursor(p_tty->p_console->cursor);
}

PUBLIC	void	scroll_screen(CONSOLE* p_con, int direction)
{
	if (direction == SCR_UP) {
		if (p_con->current_start_addr > p_con->original_addr) {
			p_con->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if (direction == SCR_DOWN) {
		if (p_con->current_start_addr + SCREEN_SIZE <
			p_con->original_addr + p_con->v_mem_limit) {
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}

	flush(p_con);
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

PRIVATE	void	set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}

PUBLIC	void	select_console(int nr_console)
{
	if (nr_console < 0 || nr_console >= NR_CONSOLES) {
		return;
	}

	current_console = nr_console;
	set_cursor(console_table[nr_console].cursor);
	set_video_start_addr(console_table[nr_console].current_start_addr);
}	

PRIVATE	void	flush(CONSOLE* p_con)
{
	if (is_current_console(p_con)) {
		set_cursor(p_con->cursor);
		set_video_start_addr(p_con->current_start_addr);
	}
}
