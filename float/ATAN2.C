#include	<math.h>

double
atan2(x, y)
double	x, y;
{
	double	v;

	if(fabs(y) >= fabs(x)) {
		v = atan(x/y);
		if( y < 0.0)
			if(x >= 0.0)
				v += 3.14159265358979;
			else
				v -= 3.14159265358979;
		return v;
	}
	v = -atan(y/x);
	if(x < 0.)
		v -= 1.57079632679489;
	else
		v += 1.57079632679489;
	return v;
}
