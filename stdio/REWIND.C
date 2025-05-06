#include	<stdio.h>

rewind(stream)
FILE *	stream;
{
	fseek(stream, 0L, 0);
}
