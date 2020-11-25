#include	"type.h"
#include	"string.h"
#include	"asm_lib.h"

PRIVATE void	integer2char(int i, int base, char** p)
{
	if (i < 0) {
		i = -i;
		*(*p)++ = '-';
	}

	int m = i % base;
	if (i / base)
		integer2char(i/base, base, p);
	if (m < 10)
		*(*p)++ = '0' + m;
	else
		*(*p)++ = 'A' + m - 10;
}		

PUBLIC	int	vsprintf(char* buf, const char* fmt, va_list args)
{
	char* p;
	char tmp[256];
	char* q = tmp;
	va_list p_next = args;

	for (p = buf; *fmt; fmt++) {
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}
		fmt++;
		
		int align_times = 0;
		char align_char = ' ';

		if (*fmt == '%') {
			*p++ = *fmt++;	
			continue;
		}

		while (*fmt == '0') {
			fmt++;
			align_char = '0';
		}

		while (*fmt >= '0' && *fmt <= '9')
			align_times = align_times * 10 + (*fmt++) - '0';

		kmemset(tmp, 0, sizeof(tmp));

		switch(*fmt) {
		case 'd':
			integer2char(*((int*) p_next), 10, &q);
			_strcpy(p, tmp);
			p_next += 4;
			p += _strlen(tmp);
			break;	
		case 'x':
			itoa(tmp, *((int*) p_next));
			_strcpy(p, tmp);
			p_next += 4;
			p += _strlen(tmp);
			break;
		case 's':
			_strcpy(p, *((char**) p_next));
			p += _strlen(*((char**) p_next));
			p_next += 4;
			break;
		default:
			break;
		}

		for (int i = 0; i < (align_times > _strlen(tmp) ? align_times - _strlen(tmp) : 0); i++)	
			*p++ = align_char;
	}
	*p = 0;

	return (p - buf);
}
