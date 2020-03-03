#include	<math.h>

double
sin(f)
double	f;
{
	static double	coeff_a[] =
	{
		 0.1357884e8,
		-0.49429081e7,
		 0.440103053e6,
		-0.138472724e5,
		 0.145968841e3
	};
	static double	coeff_b[] =
	{
		 0.864455865e7,
		 0.408179225e6,
		 0.946309610e4,
		 0.132653491e3,
		 1.0
	};
	double	y, y_squared;
	int	sect, e;
	extern double	eval_poly();

	if(f < 0.0) {
		f = -f;
		sect = 2;
	} else
		sect = 0;
	f *= .63661977237;
	if(f > 4.0)
		f -= 4.0 * floor(f/4.0);
	if(f > 2.0) {
		f -= 2.0;
		sect = 2 - sect;
	}
	y = f - (e = (int)f);
	sect = (e + sect) % 4;
	if(sect & 1)
		y = 1.0 - y;
	if(sect & 2)
		y = -y;
	y_squared = y * y;
	return y * eval_poly(y_squared, coeff_a, 4) / eval_poly(y_squared, coeff_b, 4);
}


