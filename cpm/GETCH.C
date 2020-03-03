#include	<cpm.h>

static short	pushback;

getch()
{
	short	c;

	if(c = pushback) {
		pushback = 0;
		return c;
	}
	while(!(c = bdos(0x06, 0xFF)))
		continue;
	return c;
}


getche()
{
	short	c;

	if(c = pushback) {
		pushback = 0;
		return c;
	}
	return bdos(0x01) & 0xFF;
}

ungetch(c)
unsigned char	c;
{
	pushback = c;
}

putch(c)
unsigned char	c;
{
	if(c == '\n')
		bdos(0x02, '\r');
	bdos(0x02, c);
}

kbhit()
{
	return (bdos(0x0B) & 0xFF) != 0;
}
