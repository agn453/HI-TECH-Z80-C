/*
 *	Fake routines for getchar and putchar
 */

#include	<stdio.h>

#undef	getchar
#undef	putchar
getchar()
{
	return(getc(stdin));
}

putchar(c)
{
	return(putc(c, stdout));
}
