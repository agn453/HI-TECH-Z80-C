/*
 *	Initial setup for stdio
 */

#include	<stdio.h>

char	_sibuf[BUFSIZ];
FILE	_iob[_NFILE] =
{
	{
		_sibuf,
		0,
		_sibuf,
		_IOREAD|_IOMYBUF,
		0			/* stdin */
	},
	{
		(char *)0,
		0,
		(char *)0,
		_IOWRT|_IONBF,
		1			/* stdout */
	},
	{
		(char *)0,
		0,
		(char *)0,
		_IOWRT|_IONBF,
		2			/* stderr */
	},
};
