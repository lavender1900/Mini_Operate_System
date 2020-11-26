#include	"const.h"

PUBLIC	void	_strcpy(char* ptr, char* src)
{
	char c;
	while ((c = *src++) != '\0')
		*ptr++ = c;
	*ptr = '\0';
}

PUBLIC	int	_strlen(char* p)
{
	int i = 0;
	while (*p++ != '\0')
		i++;
	return i;
}

PUBLIC char* itoa(char* str, int num)
{
	char* p = str;
	char	ch;
	int	i;
	int	flag = 0;
	
	*p++ = '0';
	*p++ = 'x';
	
	if (num == 0) {
		*p++ = '0';
	} else {
		for (i = 28; i >= 0; i -= 4) {
			ch = (num >> i) & 0xF;
			if (flag || ch > 0) {
				flag = 1;
				ch += '0';
				if (ch > '9')
					ch += 7;
				*p++ = ch;
			}
		}
	}

	*p = 0;
		
	return str;
}
