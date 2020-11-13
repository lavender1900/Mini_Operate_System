#include	"const.h"
#include	"protect.h"

void*	out_byte(int port, int ICW);

PUBLIC void init_8259A() {
 out_byte(INT_M_CTL, 0x11); // Master ICW1
 out_byte(INT_S_CTL, 0x11); // Slave ICW1
 
 out_byte(INT_M_CTLMASK, INT_VECTOR_IRQ0); // Master ICW2 - Master IRQ0 interrupt number set to 0x20
 out_byte(INT_S_CTLMASK, INT_VECTOR_IRQ8); // Slave ICW2 - Slave IRQ8 interrupt number set to 0x28

 out_byte(INT_M_CTLMASK, 0x4); // ICW3, Master chip IR2 is cascaded with slave chip
 out_byte(INT_S_CTLMASK, 0x2); // ICW3, Slave chip is mounted to 2th interrupt bus of Master chip

 out_byte(INT_M_CTLMASK, 0x1); // Master ICW4
 out_byte(INT_S_CTLMASK, 0x1); // Slave ICW4

 out_byte(INT_M_CTLMASK, 0xFF); // OCW1, Mask out all interrupts of Master chip
 out_byte(INT_S_CTLMASK, 0xFF); // OCW1, Mask out all interrupts of Slave chip
}
