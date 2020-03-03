/*
 *	Stdio scanf
 */

#include	<stdio.h>

extern int	_doscan();

scanf(fmt, args)
char *	fmt;
int	args;
{
	return _doscan(stdin, fmt, &args);
}
