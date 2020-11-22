#include	"const.h"
#include	"type.h"
#include	"proto.h"
#include	"keyboard.h"
#include	"global.h"
#include	"console.h"

PRIVATE	void	put_key(TTY* p_tty, u32 key);

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
		put_key(p_tty, key);
	} else {
		int raw_code = key & MASK_RAW;
		switch (raw_code) {
		case ENTER:
			put_key(p_tty, '\n');
			break;
		case BACKSPACE:
			put_key(p_tty, '\b');
			break;
		case UP:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
				scroll_screen(p_tty->p_console, SCR_UP);	
			}
			break;

		case DOWN:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
				scroll_screen(p_tty->p_console, SCR_DOWN);
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

PRIVATE	void	put_key(TTY* p_tty, u32 key)
{
	if (p_tty->in_buf_count < TTY_IN_BYTES) {
	
		*p_tty->p_in_buf_head++ = key;
		if (p_tty->p_in_buf_head == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_in_buf_head = p_tty->in_buf;
		}
		p_tty->in_buf_count++;
	}
}
