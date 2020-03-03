#include	<stdio.h>

/*
 *	putw for Zios
 */

putw(w, stream)
register FILE *	stream;
{
	if(putc(w&0xFF, stream) == EOF || putc((w >> 8)&0xFF, stream) == EOF)
		return(EOF);
	return(w);
}
