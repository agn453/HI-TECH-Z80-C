
#include	<math.h>

double
cosh(x)
double	x;
{
	x = exp(x);
	return 0.5*(x+1.0/x);
}
