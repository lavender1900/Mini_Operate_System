#include	"const.h"
#include	"type.h"
#include	"proto.h"
#include	"keyboard.h"
#include	"global.h"
#include	"console.h"

PRIVATE	void	init_tty(TTY* p_tty)
{
	p_tty->in_buf_count = 0;
	p_tty->p_in_buf_head = p_tty->p_in_buf_tail = p_tty->in_buf;

	init_screen(p_tty);
}

PRIVATE	void	tty_do_read(TTY* p_tty)
{
	if (is_current_console(p_tty->p_console)) {
		keyboard_read(p_tty);
	}
}

PRIVATE	void	tty_do_write(TTY* p_tty)
{
	if (p_tty->in_buf_count > 0) {
		char ch = *p_tty->p_in_buf_tail++;
		if (p_tty->p_in_buf_tail == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_in_buf_tail = p_tty->in_buf;
		}
		p_tty->in_buf_count--;

		out_char(p_tty->p_console, ch);
	}
}

PUBLIC	void	task_tty()
{
	TTY*	p_tty;
	
	for (p_tty = tty_table; p_tty < tty_table + NR_CONSOLES; p_tty++) {
		init_tty(p_tty);
	}

	select_console(0);

	while(1) {
		for (p_tty = tty_table; p_tty < tty_table + NR_CONSOLES; p_tty++) {
			tty_do_read(p_tty);
			tty_do_write(p_tty);
		}
	}
}

PUBLIC	void	in_process(TTY* p_tty, u32 key)
{
	char output[2] = {'\0', '\0'};
	
	if (!(key & FLAG_EXT)) {
		if (p_tty->in_buf_count < TTY_IN_BYTES) {
			*p_tty->p_in_buf_head++ = key;
			if (p_tty->p_in_buf_head == p_tty->in_buf + TTY_IN_BYTES) {
				p_tty->p_in_buf_head = p_tty->in_buf;
			}
			p_tty->in_buf_count++;
		}
	} else {
		int raw_code = key & MASK_RAW;
		switch (raw_code) {
		case UP:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
				disable_int();
				out_byte(CRTC_ADDR_REG, START_ADDR_H);
				out_byte(CRTC_DATA_REG, ((80*15) >> 8) & 0xFF);
				out_byte(CRTC_ADDR_REG, START_ADDR_L);
				out_byte(CRTC_DATA_REG, (80*15) & 0xFF);
				enable_int();
			}
			break;

		case DOWN:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
			// do nothing
			}
			break;
		
		case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case F11:
		case F12:
			if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R)) {
				select_console(raw_code - F1);
			}
			break;
		
		default:
			break;
		}
	}
} 
