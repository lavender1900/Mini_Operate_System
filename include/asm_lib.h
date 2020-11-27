#ifndef	_LAVENDER_ASM_LIB_H_
#define	_LAVENDER_ASM_LIB_H_

#include	"type.h"
#include	"const.h"

PUBLIC void*	kmemcpy(void* src, void* dest, int size);
PUBLIC void*	kmemset(void* dest, u8 value, int setBytes);

PUBLIC void	out_byte(u32 port, u32 value); 
PUBLIC u32	in_byte(u32 port);

PUBLIC void	disable_int();
PUBLIC void	enable_int();

PUBLIC void	enable_irq(int irq);
PUBLIC void	disable_irq(int irq);

PUBLIC	void	port_read(u16 port, void* buf, int n);
PUBLIC	void	port_write(u16 port, void* buf, int n);

#endif
