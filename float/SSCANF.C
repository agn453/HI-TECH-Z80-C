/*
 *	Stdio sscanf
 */

#include	<stdio.h>
#include	<string.h>

extern int	_doscan();

sscanf(str, fmt, args)
char *	str;
char *	fmt;
int	args;
{
	FILE	file;

	file._base = file._ptr = str;
	file._cnt = strlen(str);
	file._flag = _IOSTRG|_IOBINARY|_IOREAD;
	return _doscan(&file, fmt, &args);
}
