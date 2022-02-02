#include	<math.h>

#define	PI	3.14159265358979
#define	TWO_PI	6.28318530717958
#define	HALF_PI	1.570796326794895

double
atan2(x, y)
double	x, y;
{
	double	v;

	if(fabs(y) >= fabs(x)) {
		v = atan(x/y);
		if( y < 0.0)
			if(x >= 0.0)
				v += PI;
			else
				v -= PI;
		return v;
	}
	v = -atan(y/x);
	if(x < 0.)
		v -= HALF_PI;
	else
		v += HALF_PI;
	return v;
}
