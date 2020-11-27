#ifndef _LAVENDER_PROTO_H_
#define	_LAVENDER_PROTO_H_

#include	"type.h"
#include	"console.h"
#include	"process.h"

PUBLIC void	TestA();
PUBLIC void	TestB();
PUBLIC void	TestC();

PUBLIC	void	task_sys();

PUBLIC	void	donothing();

PUBLIC	int	sendrec(int func, int src_dest, MESSAGE* msg);
PUBLIC	int	sendint(int func, int dest, MESSAGE* msg);

PUBLIC void	sys_call();

PUBLIC int	get_ticks();
PUBLIC int	write(char* buf, int len);
PUBLIC int	printx(int unused, char* buf);

#endif
