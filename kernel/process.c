#include	"io.h"
#include	"time.h"
#include	"const.h"
#include	"proto.h"
#include	"global.h"
#include	"ipc.h"

PUBLIC	void	TestA()
{
	while(1) {
		printf("<Ticks: %5x>", get_ticks());
		assert(get_ticks() > 5000);
		milli_delay(350);
	}
}

PUBLIC	void	TestB()
{
	int i = 1;
	while(1) {
		printf("B.%3d %s", i++, "jet");
		panic("To be continue...");
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
