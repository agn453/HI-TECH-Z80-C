#include	<stdio.h>

/*
 *	Stdio getw()
 */

getw(stream)
register FILE *	stream;
{
	short	hi, lo;

	if((lo = getc(stream)) == EOF || (hi = getc(stream)) == EOF)
		return EOF;
	return (hi << 8) + (lo & 0xFF);
}
