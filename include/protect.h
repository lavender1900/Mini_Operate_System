#include	"type.h"

#ifndef _LAVENDER_PROTECT_H_
#define	_LAVENDER_PROTECT_H_

typedef struct s_descriptor
{
	u16	limit_low;
	u16	base_low;
	u8	base_mid;
	u8	attr1;		// P DPL S Type
	u8	limit_high_attr2; // G D/B 0 AVL limit_high
	u8	base_high;
} DESCRIPTOR;

typedef	struct s_gate
{
	u16	offset_low;
	u16	selector;
	u8	dcount;
	u8	attr;
	u16	offset_high;
} GATE;

#define	INT_VECTOR_IRQ0		0x20
#define	INT_VECTOR_IRQ8		0x28

#endif
