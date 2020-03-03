#include	<ctype.h>

double
atof(s)
register char *	s;
{
	char	sign;
	double	l;
	short	exp;
	short	eexp;
	char	expsign;
	double	ten;

	ten = 10.0;
	while(isspace(*s))
		s++;
	expsign = sign = 0;
	if(*s == '-') {
		s++;
		sign = 1;
	}
	l = 0;
	exp = 0;
	while(isdigit(*s))
		l = ten*l + (double)(*s++ - '0');
	if(*s == '.')
		while(isdigit(*++s)) {
			exp--;
			l = ten*l + (double)(*s - '0');
		}
	eexp = 0;
	if(*s == 'e' || *s == 'E') {
		if(*++s == '-') {
			expsign = 1;
			s++;
		}
		if(*s == '+')
			s++;
		while(isdigit(*s))
			eexp = eexp*10 + *s++ - '0';
		if(expsign)
			eexp = -eexp;
	}
	exp += eexp;
	while(exp < 0) {
		l = l / ten;
		exp++;
	}
	while(exp > 0) {
		l = l * ten;
		exp--;
	}
	if(sign)
		return -l;
	return l;
}
