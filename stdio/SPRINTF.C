#include	<stdio.h>

static	FILE	spf;

sprintf(wh, f, a)
char *	wh;
char *	f;
int	a;
{
	spf._size = 32767;
	spf._cnt = 0;
	spf._base = spf._ptr = wh;
	spf._flag = _IOWRT|_IOBINARY|_IOSTRG;
	_doprnt(&spf, f, &a);
	*spf._ptr = 0;
	return spf._ptr - wh;
}
