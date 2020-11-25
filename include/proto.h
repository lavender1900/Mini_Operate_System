#ifndef _LAVENDER_PROTO_H_
#define	_LAVENDER_PROTO_H_

#include	"type.h"
#include	"console.h"
#include	"process.h"

PUBLIC void	TestA();
PUBLIC void	TestB();
PUBLIC void	TestC();

PUBLIC	void	donothing();

PUBLIC void	sys_call();

PUBLIC int	get_ticks();
PUBLIC int	write(char* buf, int len);
PUBLIC int	printx(int unused, char* buf);

#endif
