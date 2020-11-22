#include	"type.h"
#include	"const.h"
#include	"proto.h"

PRIVATE	void	_strcpy(char* ptr, char* src)
{
	char c;
	while ((c = *src++) != '\0')
		*ptr++ = c;
}

PRIVATE	int	_strlen(char* p)
{
	int i = 0;
	while (*p++ != '\0')
		i++;
	return i;
}

PUBLIC	int	vsprintf(char* buf, const char* fmt, va_list args)
{
	char* p;
	char tmp[256];
	va_list p_next = args;

	for (p = buf; *fmt; fmt++) {
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}
		fmt++;
		
		switch(*fmt) {
		case 'x':
			itoa(tmp, *((int*) p_next));
			_strcpy(p, tmp);
			p_next += 4;
			p += _strlen(tmp);
			break;
		case 's':
			break;
		default:
			break;
		}
	}
	return (p - buf);
}
