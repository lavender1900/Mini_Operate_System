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
		send_recv(RECEIVE, ANY, &msg);
		int src = msg.source;
		donothing();
		printf("src = %d ", src);
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
