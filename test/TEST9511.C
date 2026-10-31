/*
 * Copyright (c) 2026 Phillip Stevens
 *
 * This Source Code Form is subject to the terms of the MIT License.
 * If a copy of the license was not distributed with this file,
 * You can obtain one at https://opensource.org/licenses/MIT.
 *
 * feilipu, October 2026
 */

#include <stdio.h>
#include <math.h>

/*
 * Benchmark for LIB9511 and LIBF.
 * Each float is printed as a long after scaling by 1000000.
 * Exact values use the same digits on both libraries.
 * ftol of a fraction truncates on the Am9511A.
 * LIBF ftol rounds.
 * Build the LIBF baseline with -DBASE. That build has no modf.
 */

long scale(x)
double x;
{
	return (long)(x * 1000000.0);
}

int main()
{
	double a;
	long n;
#ifndef BASE
	double ip, fp;
#endif

	printf("start\n");

	a = 1.25 + 2.5;
	printf("add %ld\n", scale(a));
	a = 1.0 - 0.5;
	printf("sub %ld\n", scale(a));
	a = 1.5 * 2.0;
	printf("mul %ld\n", scale(a));
	a = 10.0 / 4.0;
	printf("div %ld\n", scale(a));
	a = -2.5 + 1.0;
	printf("neg %ld\n", scale(a));
	a = 1.0 / 3.0;
	printf("third %ld\n", scale(a));
	a = sqrt(2.0);
	printf("sqrt2 %ld\n", scale(a));

	n = (long)2.0;
	printf("ftol2 %ld\n", n);
	n = (long)1.5;
	printf("ftol15 %ld\n", n);
	n = (long)-1.5;
	printf("ftol-15 %ld\n", n);
	n = (long)100.25;
	printf("ftol100 %ld\n", n);

	n = (long)(double)12345L;
	printf("ltof %ld\n", n);
	n = (long)(double)-12345L;
	printf("lneg %ld\n", n);
	n = (long)(double)-2;
	printf("itof %ld\n", n);
	n = (long)(double)200;
	printf("itou %ld\n", n);

#ifndef BASE
	fp = modf(3.25, &ip);
	printf("mod325 %ld %ld\n", scale(ip), scale(fp));
	fp = modf(-3.25, &ip);
	printf("mod-325 %ld %ld\n", scale(ip), scale(fp));
	fp = modf(0.5, &ip);
	printf("mod05 %ld %ld\n", scale(ip), scale(fp));
	fp = modf(-0.75, &ip);
	printf("mod-075 %ld %ld\n", scale(ip), scale(fp));
	fp = modf(8.0, &ip);
	printf("mod8 %ld %ld\n", scale(ip), scale(fp));
	fp = modf(100.5, &ip);
	printf("mod100 %ld %ld\n", scale(ip), scale(fp));
#endif
	printf("done\n");
	return 0;
}
