#include	<math.h>

#define	PI	3.14159265358979
#define	TWO_PI	6.28318530717958
#define	HALF_PI	1.570796326794895

double
cos(f)
double	f;
{
	/* cos is pi/2 out of phase with sin, so ... */

	if(f > PI)
		return sin(f - (PI+HALF_PI));
	return sin(f + HALF_PI);
}
