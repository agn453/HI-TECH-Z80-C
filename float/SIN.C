#include	<math.h>

#define	PI	3.14159265358979
#define	TWO_PI	6.28318530717958
#define	HALF_PI	1.570796326794895

double
sin(f)
double	f;
{
	/* const */ static double	coeff_a[] =
	{
		 207823.68416961012,
		-76586.415638846949,
		 7064.1360814006881,
		-237.85932457812158,
		 2.8078274176220686
	};
	/* const */ static double	coeff_b[] =
	{
		 132304.66650864931,
		 5651.6867953169177,
		 108.99981103712905,
		 1.0
	};
	double	x2;
	int	sgn;
	extern double	eval_poly();

	sgn = 0;
	if(f < 0.0) {
		f = -f;
		sgn = 1;
	}
	f *= 1.0/TWO_PI;
	f = 4.0 * (f - floor(f));
	if(f > 2.0) {
		f -= 2.0;
		sgn = !sgn;
	}
	if( f > 1.0)
		f = 2.0 - f;
	x2 = f * f;
	f *= eval_poly(x2, coeff_a, 4) / eval_poly(x2, coeff_b, 3);
	if(sgn)
		return -f;
	return f;
}


