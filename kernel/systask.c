#include	"type.h"
#include	"const.h"
#include	"ipc.h"
#include	"api.h"
#include	"global.h"
#include	"io.h"
#include	"message.h"

PUBLIC	void	task_sys()
{
	MESSAGE msg;
	while (1) {
/*		if (proc_table[6].p_flags == RECEIVING && proc_table[6].p_recvfrom == 5
			&& proc_table[5].p_flags == SENDING && proc_table[5].p_sendto == 4)
			printf("Scheduled ");
*/		send_recv(RECEIVE, ANY, &msg);
		int src = msg.source;
		switch (msg.type) {
		case GET_TICKS:
			msg.RETVAL = ticks;
			send_recv(SEND, src, &msg);
			break;
		default:
			panic("unknown msg type %d", msg.type);
			break;
		}
	}
}
