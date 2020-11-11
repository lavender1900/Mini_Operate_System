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

#endif
