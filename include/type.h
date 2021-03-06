#ifndef	_LAVENDER_TYPE_H_
#define	_LAVENDER_TYPE_H_

typedef	unsigned long long	u64;
typedef	unsigned int		u32;
typedef unsigned short		u16;
typedef	unsigned char		u8;

typedef	void	(* int_handler)();
typedef	void	(* irq_handler)(int irq);
typedef	void*	system_call;

typedef	char*	va_list;

#endif
