#ifndef	_LAVENDER_8259A_H_
#define	_LAVENDER_8259A_H_

#include	"const.h"

#define	INT_M_CTL	0x20 // Write to this port enable initialize 8259A master chip
#define	INT_M_CTLMASK	0x21 // Write to this port enable control interrupt response of master chip
#define	INT_S_CTL	0xA0 // Write to this port enable initialize 8259A slave chip
#define	INT_S_CTLMASK	0xA1 // Write to this port enable control interrupt response of slave chip

PUBLIC	void	init_8259A();

#endif
