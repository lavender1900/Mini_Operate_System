#include	"type.h"
#include	"const.h"
#include	"proto.h"

PUBLIC	void	keyboard_handler(int irq)
{
	disp_str("*");
}

PUBLIC	void	init_keyboard()
{
	add_irq_handler(KEYBOARD_IRQ, keyboard_handler);
	enable_irq(KEYBOARD_IRQ);
}
