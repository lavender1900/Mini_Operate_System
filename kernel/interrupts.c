#include	"type.h"
#include	"global.h"
#include	"io.h"

PUBLIC	void	add_irq_handler(int irq, irq_handler handler)
{
	irq_table[irq] = handler;
}

PUBLIC void spurious_irq(int irq)
{
	disp_str("spurious_irq: ");
	disp_int(irq);
}
