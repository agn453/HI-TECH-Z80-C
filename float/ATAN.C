#include	<math.h>

double
atan(f)
double	f;
{
	static double	coeff_a[] =
	{
		33.05861847399,
		58.655751569,
		32.3907948562,
		5.853195211263,
		0.1952374193623,
		-.002434603300441
	};
	static double	coeff_b[] =
	{
		33.05861847399,
		69.67529105952,
		49.00434821822,
		12.97557886271,
		1.0
	};
	int	recip;
	extern double	eval_poly(), fabs();
	double		val, val_squared;

	if((val = fabs(f)) == 0.0)
		return 0.0;
	if(recip = (val > 1.0))
		val = 1.0/val;
	val_squared = val * val;
	val *= eval_poly(val_squared, coeff_a, 5)/eval_poly(val_squared, coeff_b, 4);
	if(recip)
		val = 1.570796326795 - val;
	return f < 0.0 ? -val : val;
}
