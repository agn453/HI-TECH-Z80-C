#include	<conio.h>

char *
cgets(s)
char *	s;
{
	char *	s1 = s;
	int	c;

	while((c = getche()) != '\r' &&  c != '\n')
		*s++ = c;
	*s = 0;
	if(s == s1)
		return((char *)0);
	return(s1);
}
