#include	"type.h"
#include	"const.h"
#include	"global.h"
#include	"proto.h"
#include	"process.h"

void	incinc();

PUBLIC	void	TestA()
{
	int i = 0;
	while(1) {
		disp_str("A");
		disp_int(i++);
		disp_str(".");
		delay(1);
	}
}

PUBLIC	void	TestB()
{
	int i = 1900;
	while(1) {
		disp_str("B");
		disp_int(i++);
		disp_str(".");
		delay(1);
	}
}
