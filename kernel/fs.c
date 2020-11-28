#include	"global.h"
#include	"ipc.h"
#include	"hd.h"
#include	"io.h"

PUBLIC	void	task_fs()
{

	printf("Task FS starts");
	MESSAGE	msg;
	msg.type = DEV_OPEN;
	msg.DEVICE = MINOR(ROOT_DEV);
	assert(dev_drv_map[MAJOR(ROOT_DEV)].driver_handler != INVALID_DRIVER);

	send_recv(BOTH, dev_drv_map[MAJOR(ROOT_DEV)].driver_handler, &msg);
	
	while(1) {}
}
