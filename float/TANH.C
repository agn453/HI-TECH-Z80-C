
#include	<math.h>

double
tanh(x)
double	x;
{
	x = exp(x);
	return (x-1.0/x)/(x+1.0/x);
}
