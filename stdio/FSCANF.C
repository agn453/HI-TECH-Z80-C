/*
 *	Stdio fscanf
 */

#include	<stdio.h>
extern int	_doscan();

fscanf(file, fmt, args)
FILE *	file;
char *	fmt;
int	args;
{
	return _doscan(file, fmt, &args);
}
