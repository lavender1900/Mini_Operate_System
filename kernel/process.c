#include	"io.h"
#include	"time.h"
#include	"const.h"
#include	"proto.h"

PUBLIC	void	TestA()
{
	while(1) {
		printf("<Ticks: %x>", get_ticks());
		milli_delay(350);
	}
}

PUBLIC	void	TestB()
{
	while(1) {
		printf("B.");
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

