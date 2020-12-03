#include	"type.h"
#include	"const.h"
#include	"process.h"
#include	"message.h"
#include	"ipc.h"
#include	"string.h"

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

PUBLIC	int	open(const char* pathname, int flags)
{
	MESSAGE	msg;
	msg.type = OPEN;
	msg.PATHNAME = (void*) pathname;
	msg.FLAGS = flags;
	msg.NAME_LEN = _strlen(pathname);

	send_recv(BOTH, TASK_FS, &msg);
	assert(msg.type == SYSCALL_RET);

	return msg.FD;
}

PUBLIC	void	close(int fd)
{
	MESSAGE msg;
	msg.type = CLOSE;
	msg.FD = fd;

	send_recv(BOTH, TASK_FS, &msg);
}

PUBLIC	int	file_read(int fd, void* buf, int count)
{
	MESSAGE	msg;
	msg.type = READ;
	msg.FD = fd;
	msg.BUF = buf;
	msg.CNT = count;

	send_recv(BOTH, TASK_FS, &msg);
	
	return msg.CNT;
}

PUBLIC	int	file_write(int fd, void* buf, int count)
{
	MESSAGE	msg;
	msg.type = WRITE;
	msg.FD = fd;
	msg.BUF = buf;
	msg.CNT = count;

	send_recv(BOTH, TASK_FS, &msg);

	return msg.CNT;
}
