#include	<cpm.h>

static short	pushback;

getch()
{
	short	c;

	if(c = pushback) {
		pushback = 0;
		return c;
	}
	while(!(c = bdos(CPMDCIO, 0xFF)))
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
	return bdos(CPMRCON) & 0xFF;
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
		bdos(CPMWCON, '\r');
	bdos(CPMWCON, c);
}

kbhit()
{
	return (bdos(CPMICON) & 0xFF) != 0;
}
