#include	"process.h"
#include	"global.h"
#include	"console.h"
#include	"ipc.h"
#include	"page.h"
#include	"asm_lib.h"
#include	"io.h"

PUBLIC	int	sys_get_ticks()
{
	return ticks;
}

PUBLIC	int	sys_write(char* buf, int len, PROCESS* p_proc)
{
	tty_write(&tty_table[p_proc->nr_tty], buf, len);
	return 0;
}

PUBLIC	int	sys_sendrec(int function, int src_dest, PROCESS* p,  MESSAGE* msg)
{
	assert(k_reenter == 0);
	assert((0 <= src_dest && NR_TASKS+NR_PROCS > src_dest) || ANY == src_dest || INTERRUPT == src_dest);
	
	int ret = 0;
	int caller = proc2pid(p);
	MESSAGE* mla = (MESSAGE*) vir2linear(ldt_proc_id2base(caller), msg);
	mla->source = caller;

	assert(mla->source != src_dest); // cannot send to or receive from self

	if (function == SEND) {
		ret = msg_send(p, src_dest, msg);
		if (ret != 0)
			return ret;
	}
	else if (function == RECEIVE) {
		ret = msg_receive(p, src_dest, msg);
		if (ret != 0)
			return ret;
	}
	else {
		panic("{sys_sendrec} invalid function: %d (SEND:  %d, RECEIVE: %d).", function, SEND, RECEIVE);
	}

	return 0;
}	

PUBLIC	int	sys_printx(/*int _unused1,*/ int _unused2, char* s, PROCESS* p_proc)
{
	const char* p;
	char ch;

	char reenter_err[] = "k_reenter is incorrect for unknown reason";

	if (k_reenter == 0)
		p = (char*) vir2linear(ldt_proc_id2base(proc2pid(p_proc)), s);
	else if (k_reenter > 0)
		p = s;
	else
		p = reenter_err;

	// If panic() or assert() is called from TASK process, then halt the whole system
	if (p_proc->proc_type == PROC_TYPE_TASK) {
		disable_int();
		char* v = (char*) V_MEM_BASE;
		const char* q = p;
		while (v < (char*) (V_MEM_BASE + V_MEM_SIZE)) {
			*v++ = *q++;
			*v++ = RED_CHAR;
			if (!*q) {
				while (((int) v - V_MEM_BASE) % (SCREEN_WIDTH * 16)) {
					v++;
					*v++ = GRAY_CHAR;
				}
				q = p;
			}
		}

		__asm__ __volatile__("hlt");
	}

	while ((ch = *p++) != 0) {
		out_char(tty_table[p_proc->nr_tty].p_console, ch);
	}

	return 0;
}
