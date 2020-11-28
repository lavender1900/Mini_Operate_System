#include	"type.h"
#include	"const.h"
#include	"process.h"
#include	"message.h"
#include	"ipc.h"

PUBLIC	int	get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}

PUBLIC	int	get_hd_info()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = DEV_OPEN;
	send_recv(BOTH, TASK_HD, &msg);
	return msg.RETVAL;
}
