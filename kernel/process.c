#include	"io.h"
#include	"time.h"
#include	"const.h"
#include	"proto.h"
#include	"global.h"
#include	"ipc.h"
#include	"api.h"

PUBLIC	void	TestA()
{
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
