#include	<math.h>

double
acos(x)
double	x;
{
	double	y;

	y = sqrt(1 - x*x);
	return atan(y/x);
}
