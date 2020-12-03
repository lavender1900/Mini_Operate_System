#include	"type.h"
#include	"const.h"
#include	"io.h"
#include	"proto.h"
#include	"ipc.h"
#include	"asm_lib.h"
#include	"global.h"
#include	"page.h"
#include	"clock.h"

PUBLIC	int	panic(char* fmt, ...)
{
	char buf[1024];
	va_list args = (va_list)((char*)&fmt + 4);
	int i = vsprintf(buf, fmt, args);
	
	return printx(0, buf);
}	

PUBLIC	void	assertion_failure(char* exp, char* file, char* base_file, int line)
{
	panic("assert(%s) failed: file: %s, base_file: %s, line %d",
		exp, file, base_file, line);

	while(1) {}

	// never walk here
	__asm__ __volatile__("ud2");
}

PUBLIC	void	reset_msg(MESSAGE* p)
{
	kmemset(p, 0, sizeof(MESSAGE));
}

PUBLIC	void	block(PROCESS* p)
{
	assert(p->p_flags != 0);
	schedule();
}

PUBLIC	void	unblock(PROCESS* p)
{
	assert(0 == p->p_flags);
}

PUBLIC	int	deadlock(int src, int dest)
{
	PROCESS* p = proc_table + dest;
	while (1) {
		if (p->p_flags & SENDING) {
			if (p->p_sendto == src) {
				p = proc_table + dest;
				printf("=_=%s", p->name);
				do {
					assert(p->p_msg);
					p = proc_table + p->p_sendto;
					printf("->%s", p->name);
				} while (p != proc_table + src);
				printf("=_=");
				return 1;
			}
			p = proc_table + p->p_sendto;
		} else {
			break;
		}
	}

	return 0;
}	

PUBLIC	int	inform_int(int dest)
{
	MESSAGE msg;
	msg.source = INTERRUPT;
	msg.type = HARD_INT;
	return sendint(SEND, dest, &msg);	
}
	

PUBLIC	int	send_recv(int function, int src_dest, MESSAGE* msg)
{
	int ret = 0;

	if (function == RECEIVE)
		kmemset(msg, 0, sizeof(MESSAGE));

	switch(function) {
	case BOTH:
		ret = sendrec(SEND, src_dest, msg);
		if (ret == 0)
			ret = sendrec(RECEIVE, src_dest, msg);
		break;
	case SEND:
	case RECEIVE:
		ret = sendrec(function, src_dest, msg);
		break;
	default:
		assert(function == BOTH || function == SEND || function == RECEIVE);
		break;
	}

	return ret;
}

PUBLIC	int	msg_send(PROCESS* current, int dest, MESSAGE* m)
{
	PROCESS* p_sender = current;
	PROCESS* p_dest = proc_table + dest;

	assert(proc2pid(p_sender) != dest);

	if (deadlock(proc2pid(p_sender), dest)) {
		panic(">>DEADLOCK<< %s->%s", p_sender->name, p_dest->name);
	}

	/* dest is waiting for message */
	if ((p_dest->p_flags & RECEIVING) &&
		(p_dest->p_recvfrom == proc2pid(p_sender) || p_dest->p_recvfrom == ANY)) {
		assert(p_dest->p_msg);
		assert(m);

		kmemcpy((void*)vir2linear(ldt_proc_id2base(proc2pid(p_sender)), m),
			(void*)vir2linear(ldt_proc_id2base(dest), p_dest->p_msg),
			sizeof(MESSAGE));

		p_sender->p_msg = 0;
		p_dest->p_msg = 0;
		p_dest->p_flags &= ~RECEIVING;
		p_dest->p_recvfrom = NO_TASK;

		unblock(p_dest);

		assert(p_dest->p_flags == 0);
		assert(p_dest->p_msg == 0);
		assert(p_dest->p_recvfrom == NO_TASK);
		assert(p_dest->p_sendto == NO_TASK);
		assert(p_sender->p_flags == 0);
		assert(p_sender->p_msg == 0);
		assert(p_sender->p_recvfrom == NO_TASK);
		assert(p_sender->p_sendto == NO_TASK);
	}
	else {

		/* dest is not waiting for message */
		p_sender->p_flags |= SENDING;
		assert(p_sender->p_flags == SENDING);
		p_sender->p_sendto = dest;
		p_sender->p_msg = m;

		PROCESS* p;
		if (p_dest->q_sending) {
			p = p_dest->q_sending;
			while (p->next_sending)
				p = p->next_sending;
			p->next_sending = p_sender;
		}
		else {
			p_dest->q_sending = p_sender;
		}
		p_sender->next_sending = 0;

		assert(p_sender->p_flags != 0);
		
		block(p_sender);
		assert(p_sender->p_flags != 0);

		assert(p_sender->p_flags == SENDING);
		assert(p_sender->p_msg != 0);
		assert(p_sender->p_recvfrom == NO_TASK);
		assert(p_sender->p_sendto == dest);
	}

	return 0;
}

PUBLIC	int	msg_receive(PROCESS* current, int src, MESSAGE* m)
{
	PROCESS* p_who_wanna_recv = current;
	PROCESS* p_from = 0;
	PROCESS* prev = 0;
	int copyok = 0;

	assert(proc2pid(p_who_wanna_recv) != src);

	if ((p_who_wanna_recv->has_int_msg) &&
		((src == ANY) || (src == INTERRUPT))) {
		assert(p_who_wanna_recv);
		kmemcpy((void*)vir2linear(ldt_proc_id2base(proc2pid(p_who_wanna_recv)), p_who_wanna_recv->p_msg),
			(void*)vir2linear(ldt_proc_id2base(proc2pid(p_who_wanna_recv)), m),
			sizeof(MESSAGE));
		p_who_wanna_recv->has_int_msg = 0;
		p_who_wanna_recv->p_msg = 0;
		
		assert(p_who_wanna_recv->p_flags == 0);
		assert(p_who_wanna_recv->p_msg == 0);
		assert(p_who_wanna_recv->p_sendto == NO_TASK);
		assert(p_who_wanna_recv->has_int_msg == 0);

		return 0;
	}


	if (src == ANY) {
		if (p_who_wanna_recv->q_sending) {
			p_from = p_who_wanna_recv->q_sending;
			copyok = 1;
			assert(p_who_wanna_recv->p_flags == 0);
			assert(p_who_wanna_recv->p_msg == 0);
			assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
			assert(p_who_wanna_recv->p_sendto == NO_TASK);
			assert(p_who_wanna_recv->q_sending != 0);
			assert(p_from->p_flags == SENDING);
			assert(p_from->p_msg != 0);
			assert(p_from->p_recvfrom == NO_TASK);
			assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
		}
	}
	else if(src >= 0 && src < NR_TASKS+NR_PROCS) {
		p_from = &proc_table[src];
		if ((p_from->p_flags & SENDING) &&
			(p_from->p_sendto == proc2pid(p_who_wanna_recv))) {
			copyok = 1;
			PROCESS* p = p_who_wanna_recv->q_sending;
			assert(p);
			while(p) {
				assert(p_from->p_flags & SENDING);
				if (proc2pid(p) == src)
					break;

				prev = p;
				p = p->next_sending;
			}

			assert(p_who_wanna_recv->p_flags == 0);
			assert(p_who_wanna_recv->p_msg == 0);
			assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
			assert(p_who_wanna_recv->p_sendto == NO_TASK);
			assert(p_who_wanna_recv->q_sending != 0);
			assert(p_from->p_flags == SENDING);
			assert(p_from->p_msg != 0);
			assert(p_from->p_recvfrom == NO_TASK);
			assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
		}
	}

	if (copyok) {
		if (p_from == p_who_wanna_recv->q_sending) {
			assert(prev == 0);
			p_who_wanna_recv->q_sending = p_from->next_sending;
			p_from->next_sending = 0;
		}
		else {
			assert(prev);
			prev->next_sending = p_from->next_sending;
			p_from->next_sending = 0;
		}

		assert(m);
		assert(p_from->p_msg);
	

		kmemcpy((void*)vir2linear(ldt_proc_id2base(proc2pid(p_from)), p_from->p_msg),
			(void*)vir2linear(ldt_proc_id2base(proc2pid(p_who_wanna_recv)), m),
			sizeof(MESSAGE));

		p_from->p_msg = 0;
		p_from->p_sendto = NO_TASK;
		p_from->p_flags &= ~SENDING;

		unblock(p_from);
	}
	else {
		p_who_wanna_recv->p_flags |= RECEIVING;
		assert(p_who_wanna_recv->p_flags != 0);
		p_who_wanna_recv->p_msg = m;
		p_who_wanna_recv->p_recvfrom = src;
		block(p_who_wanna_recv);

		assert(p_who_wanna_recv->p_flags == RECEIVING);
		assert(p_who_wanna_recv->p_msg != 0);
		assert(p_who_wanna_recv->p_recvfrom != NO_TASK);
		assert(p_who_wanna_recv->p_sendto == NO_TASK);
		assert(p_who_wanna_recv->has_int_msg == 0);
	}

	return 0;
}
