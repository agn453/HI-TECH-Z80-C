#include <conio.h>

srand1(s)
char *	s;
{
	int	i;
	while(*s)
		putchar(*s++);
	while(!kbhit())
		i++;
	srand(i);
}
