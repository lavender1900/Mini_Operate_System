#include	"io.h"
#include	"time.h"
#include	"const.h"
#include	"proto.h"
#include	"global.h"
#include	"ipc.h"
#include	"api.h"
#include	"message.h"
#include	"fs.h"

PUBLIC	void	TestA()
{
	MESSAGE msg;
	msg.source = 0;
	msg.type = OPEN;
	msg.FLAGS = O_CREAT;
	msg.PATHNAME = "/blabla";
	msg.NAME_LEN = 7;
	send_recv(BOTH, TASK_FS, &msg);

	int i = 0;
	while(1) {
		printf("<Ticks: %5x>", get_ticks());
		milli_delay(150);
	}
}

PUBLIC	void	TestB()
{
	int i = 1;
	while(1) {
		printf("I'm B");
		milli_delay(400);
	}
}

PUBLIC	void	TestC()
{
	while(1) {
		printf("C.");
		milli_delay(500);
	}
}
