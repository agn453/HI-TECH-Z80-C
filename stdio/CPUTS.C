#include	<conio.h>

void cputs(s)
register char *	s;
{
	while(*s)
		putch(*s++);
}
