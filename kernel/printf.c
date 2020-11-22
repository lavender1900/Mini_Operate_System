#include	"type.h"
#include	"const.h"
#include	"proto.h"

PUBLIC	int	vsprintf(char* buf, const char* fmt, va_list args);

PUBLIC	int	printf(const char* fmt, ...)
{
	char buf[256];

	va_list arg = (va_list) ((char*) (&fmt) + 4);
	int i = vsprintf(buf, fmt, arg);
	write(buf, i);

	return i;
}
