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
	char* filename[] = {"/foo", "/bar", "/baz"};
	int fd;
	for (int j = 0; j < 3; j++) {
		fd = open(filename[j], O_CREAT | O_RDWR);
		assert(fd != -1);
		printf("File created %s (fd %d)\n", filename[j], fd);
		close(fd);
	}

	char* rmfiles[] = {"/foo", "bar", "/baz", "/dev_tty0"};
	for (int j = 0; j < 4; j++) {
		if (unlink(rmfiles[j]) == 0)
			printf("File removed: %s\n", rmfiles[j]);
		else
			printf("Failed to remove file %s\n", rmfiles[j]);
	}

	int i = 0;
	while(1) {
//		printf("<Ticks: %5x>", get_ticks());
		milli_delay(350);
	}
}

PUBLIC	void	TestB()
{
	printf("B starting");
	int i = 1;
	int fd;
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
