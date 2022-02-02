#include	<math.h>

#define	PI	3.14159265358979
#define	TWO_PI	6.28318530717958
#define	HALF_PI	1.570796326794895

double
acos(x)
double	x;
{
	return HALF_PI - asin(x);
}
