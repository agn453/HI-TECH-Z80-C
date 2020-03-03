#include	<math.h>

double
sqrt(x)
double	x;
{
	double	og, ng;
	short	niter;
	int	exp;

	if(x <= 0.0)
		return 0.0;
	og = x;
	if(og < 1.0)
		og = 1.0/og;
	og = frexp(og, &exp);
	og = ldexp(og, exp/2);		/* make an educated guess */
	if(x < 1.0)
		og = 1.0/og;
	for(niter = 0 ; niter < 20 ; niter++) {
		ng = (x/og + og)/2.0;
		if(ng == og)
			break;
		og = ng;
	}
	return og;
}
