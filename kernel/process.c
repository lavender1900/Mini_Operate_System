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
//	send_recv(BOTH, TASK_FS, &msg);

	int i = 0;
	while(1) {
		int t = get_ticks();
		donothing();
		printf("<Ticks: %5x>", t);
		milli_delay(350);
	}
}

PUBLIC	void	TestB()
{
	printf("B starting");
	int i = 1;
/*	int fd;
	int n;
	const char filename[] = "blah";
	const char bufw[] = "abcdefsdfsdf";
	const int rd_bytes = 3;
	char bufr[rd_bytes+1];
	fd = open(filename, O_CREAT | O_RDWR);
	printf("File created. fd = %d\n", fd);

	n = file_write(fd, bufw, _strlen(bufw));
	close(fd);

	fd = open(filename, O_RDWR);
	printf("File opened. fd = %d\n", fd);

	n = file_read(fd, bufr, rd_bytes);
	bufr[n] = 0;
	printf("Read file: %s", bufr);

	close(fd);
*/
	while(1) {
//		printf("I'm B");
//		milli_delay(400);
	}
}

PUBLIC	void	TestC()
{
	while(1) {
		printf("C.");
		milli_delay(500);
	}
}
