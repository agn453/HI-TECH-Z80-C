/*
 *	puts and fputs for HI-TECH C stdio
 */

#include	<stdio.h>

fputs(s, f)
char *		s;
register FILE *	f;
{
	while(*s)
		if(putc(*s++, f) == EOF)
			return EOF;
	return 0;
}

puts(s)
char *		s;
{
	register int	i;

	i = fputs(s, stdout);
	putchar('\n');
	return i;
}
