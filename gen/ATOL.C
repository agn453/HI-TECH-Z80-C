#include	<ctype.h>

long
atol(s)
register char *	s;
{
	long	a;
	unsigned char	sign;

	while(*s == ' ' || *s == '\t')
		s++;
	a = 0;
	sign = 0;
	if(*s == '-') {
		sign++;
		s++;
	} else if(*s == '+')
		s++;
	while(isdigit(*s))
		a = a*10L + (*s++ - '0');
	if(sign)
		return -a;
	return a;
}
