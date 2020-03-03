#include	<math.h>

double
asin(x)
double	x;
{
	double	y;

	y = sqrt(1 - x*x);
	return atan(x/y);
}
