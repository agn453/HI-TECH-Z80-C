/*
 *	gets and fgets for Zios stdio
 */

#include	<stdio.h>
#include	<string.h>

char *
fgets(s, n, f)
char *		s;
register FILE *	f;
{
	char *	s1 = s;
	int	c;

	while(n-- && (c = getc(f)) != EOF && (*s++ = c) != '\n')
		/* VOID */;
	*s = 0;
	if(s == s1)
		return((char *)NULL);
	return(s1);
}

char *
gets(s)
char *	s;
{
	if((s = fgets(s, -1, stdin)) == (char *)NULL)
		return((char *)NULL);
	s[strlen(s)-1] = 0;
	return(s);
}
