#include	<stdio.h>

extern int	_doprnt();

printf(f, a)
char *	f;
int	a;
{
	return(_doprnt(stdout, f, &a));
}
