#include	"keyboard.h"
#include	"keymap.h"
#include	"tty.h"
#include	"asm_lib.h"
#include	"interrupts.h"
#include	"global.h"
#include	"ipc.h"

PRIVATE	KB_INPUT	kb_in;
PRIVATE int		code_with_E0;
PRIVATE int		shift_l;
PRIVATE	int		shift_r;
PRIVATE	int		alt_l;
PRIVATE	int		alt_r;
PRIVATE	int		ctrl_l;
PRIVATE	int		ctrl_r;
PRIVATE	int		caps_lock;
PRIVATE	int		num_lock;
PRIVATE	int		scroll_lock;
PRIVATE	int		column;

PRIVATE	u8		get_byte_from_kbuf();
PRIVATE	void		kb_wait();
PRIVATE	void		kb_ack();
PRIVATE	void		set_leds();
PRIVATE	void		keyboard_handler(int irq);


PUBLIC	void	init_keyboard()
{
	kb_in.count = 0;
	kb_in.p_head = kb_in.p_tail = kb_in.buf;
	
	code_with_E0 = shift_l = shift_r = alt_l = alt_r = ctrl_l = ctrl_r = 0;
	caps_lock = scroll_lock = 0;
	num_lock = 1; 

	set_leds();

	add_irq_handler(KEYBOARD_IRQ, keyboard_handler);
	enable_irq(KEYBOARD_IRQ);
}

PUBLIC	void	keyboard_read(TTY* p_tty)
{
	u8	scan_code;
	int	make;

	u32	key = 0;

	u32*	keyrow;

	if (kb_in.count > 0)
	{
		scan_code = get_byte_from_kbuf();
		
		if (scan_code == 0xE1) {
			u8 pausebrk_scode[] = { 0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5 };
			int is_pausebreak = 1;
			
			for (int i = 1; i < 6; i++) {
				if (get_byte_from_kbuf() != pausebrk_scode[i]) {
					is_pausebreak = 0;
					break;
				}
			}
			if (is_pausebreak) {
				key = PAUSEBREAK;
			}
		} else if (0xE0 == scan_code) {
			scan_code = get_byte_from_kbuf();
			if (scan_code == 0x2A) {
				if (get_byte_from_kbuf() == 0xE0) {
					if (get_byte_from_kbuf() == 0x37) {
						key = PRINTSCREEN;
						make = 1;
					}
				}
			}

			if (0xB7 == scan_code) {
				if (get_byte_from_kbuf() == 0xE0) {
					if (get_byte_from_kbuf() == 0xAA) {
						key = PRINTSCREEN;
						make = 0;
					}
				}
			}

			if (key == 0) {
				code_with_E0 = 1;
			}	
		} 
		if (key != PAUSEBREAK && key != PRINTSCREEN) {
			make = (scan_code & FLAG_BREAK) ? 0 : 1;
		
			keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];

			column = 0;

			int caps = shift_l || shift_r;

			if (caps_lock) {
				if ((keyrow[0] >= 'a') && (keyrow[0] <= 'z')) {
					caps = !caps;
				}
			}

			if (caps) {
				column = 1;
			}

			if (code_with_E0) {
				column = 2;
				code_with_E0 = 0;
			}

			key = keyrow[column];

			switch(key) {
			case SHIFT_L:
				shift_l = make;
				break;
			case SHIFT_R:
				shift_r = make;
				break;
			case CTRL_L:
				ctrl_l = make;
				break;
			case CTRL_R:
				ctrl_r = make;
				break;
			case ALT_L:
				alt_l = make;
				break;
			case ALT_R:
				alt_r = make;
				break;
			case CAPS_LOCK:
				if (make) {
					caps_lock = !caps_lock;
					set_leds();
				}
				break;
			case NUM_LOCK:
				if (make) {
					num_lock = !num_lock;
					set_leds();
				}
				break;
			case SCROLL_LOCK:
				if (make) {
					scroll_lock = !scroll_lock;
					set_leds();
				}
				break;

			default:
				break;
			}	

			if (make) {
				int pad = 0;
				if ((key >= PAD_SLASH) && (key <= PAD_9)) {
					pad = 1;
					switch(key) {
					case PAD_SLASH:
						key = '/';
						break;
					case PAD_STAR:
						key = '*';
						break;
					case PAD_MINUS:
						key = '-';
						break;
					case PAD_PLUS:
						key = '+';
						break;
					case PAD_ENTER:
						key = ENTER;
						break;
					default:
						if (num_lock && (key >= PAD_0) && (key <= PAD_9)) {
							key = key - PAD_0 + '0';
						}
						else if (num_lock && key == PAD_DOT) {
							key = '.';
						}
						else {
							switch(key) {
							case PAD_HOME:
								key = HOME;
								break;
							case PAD_END:
								key = END;
								break;
							case PAD_PAGEUP:
								key = PAGEUP;
								break;
							case PAD_PAGEDOWN:
								key = PAGEDOWN;
								break;
							case PAD_INS:
								key = INSERT;
								break;
							case PAD_UP:
								key = UP;
								break;
							case PAD_DOWN:
								key = DOWN;
								break;
							case PAD_LEFT:
								key = LEFT;
								break;
							case PAD_RIGHT:
								key = RIGHT;
								break;
							case PAD_DOT:
								key = DELETE;
								break;
							default:
								break;
							}
						}
						break;
					}
				}

				key |= (shift_l ? FLAG_SHIFT_L : 0);
				key |= (shift_r ? FLAG_SHIFT_R : 0);
				key |= (ctrl_l ? FLAG_CTRL_L : 0);
				key |= (ctrl_r ? FLAG_CTRL_R : 0);
				key |= (alt_l ? FLAG_ALT_L : 0);
				key |= (alt_r ? FLAG_ALT_R : 0);
				key |= (pad ? FLAG_PAD : 0);

				tty_input_process(p_tty, key); 
			}
		}
	}
}

PRIVATE	u8	get_byte_from_kbuf() {
	while (kb_in.count <= 0)
		;

	disable_int();
	u8 scan_code = *kb_in.p_tail++;

	if (kb_in.p_tail == kb_in.buf + KB_IN_BYTES)
		kb_in.p_tail = kb_in.buf;

	kb_in.count--;
	enable_int();

	return scan_code;
}

PRIVATE	void	keyboard_handler(int irq)
{
	for (int i = 0; i < NR_PROCS + NR_TASKS; i++) {
		PROCESS* p = &proc_table[i];
		while (p->p_sendto != NO_TASK) {
			p = &proc_table[p->p_sendto];
			if (proc2pid(p) == i) {
				p = &proc_table[i];
				printf("Has circle: %d -> ", i);
				while (p->p_sendto != NO_TASK) {
					p = &proc_table[p->p_sendto];
					printf("%d -> ", proc2pid(p));
					if (proc2pid(p) == i)
						break;
				}
				break;
			}
		}

		p = &proc_table[i];
		while (p->p_flags & SENDING) {
			p = &proc_table[p->p_sendto];
			if (p->p_flags & RECEIVING) {
				p = &proc_table[i];
				printf("Has conflict: %d -> ", i);
				while (p->p_flags & SENDING) {
					p = &proc_table[p->p_sendto];
					if (p->p_flags & SENDING) {
						printf("%d -> ", proc2pid(p));
					} else if (p->p_flags & RECEIVING) {
						printf("%d !<- ", proc2pid(p));
						break;
					}
				}
				break;
				printf("\n");
			}
		}
	}

	for (int i = 0; i < NR_PROCS + NR_TASKS; i++) {
		printf("%d flags = %d ", i, proc_table[i].p_flags);
		if (proc_table[i].p_sendto != NO_TASK)
			printf("sendto -> %d ", proc_table[i].p_sendto);
		if (proc_table[i].p_recvfrom != NO_TASK)
			printf("recvfrom <- %d ", proc_table[i].p_recvfrom);
		printf("\n");
	}

	u8 scan_code = in_byte(0x60);

	if (kb_in.count < KB_IN_BYTES)
	{
		*(kb_in.p_head) = scan_code;
		kb_in.p_head++;
		if (kb_in.p_head == kb_in.buf + KB_IN_BYTES)
			kb_in.p_head = kb_in.buf;
		kb_in.count++;
	}
}

// ************ Wait 8042 Keyboard Controller buffer empty *****************
PRIVATE	void	kb_wait()
{
	u8 kb_stat;
	
	do {
		kb_stat = in_byte(KB_CMD);
	} while (kb_stat & 0x02);
}

// ************ 8048 Keyboard Encoder ACK ****************
PRIVATE	void	kb_ack()
{
	u8 kb_read;
	
	do {
		kb_read = in_byte(KB_DATA);
	} while(kb_read != KB_ACK);
}

PRIVATE	void	set_leds()
{
	u8 leds = (caps_lock << 2) | (num_lock << 1) | scroll_lock;
	
	kb_wait();
	out_byte(KB_DATA, LED_CODE);
	kb_ack();

	kb_wait();
	out_byte(KB_DATA, leds);
	kb_ack();
}
