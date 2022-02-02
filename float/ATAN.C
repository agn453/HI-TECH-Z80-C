#include	<math.h>

#define	PI	3.14159265358979
#define	TWO_PI	6.28318530717958
#define	HALF_PI	1.570796326794895

double
atan(f)
double	f;
{
	static const double	coeff_a[] =
	{
		33.058618473989548,
		58.655751569001961,
		32.390974856200445,
		5.8531952112628600,
		0.19523741936234277,
		-.0024346033004411264
	};
	static const double	coeff_b[] =
	{
		33.058618473992416,
		69.675291059524653,
		49.004348218216250,
		12.975578862709239,
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
		val = HALF_PI - val;
	return f < 0.0 ? -val : val;
}
