#ifndef	_LAVENDER_PAGE_H_
#define	_LAVENDER_PAGE_H_

#define	vir2linear(seg_base, vir) (u32)((u32)seg_base + (u32)vir)

PUBLIC	u32	selector2base(u16 seg);
PUBLIC	u32	ldt_proc_id2base(int proc_id);

#endif
