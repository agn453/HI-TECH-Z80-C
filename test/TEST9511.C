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
#ifndef BASE
#include <am9511.h>
#endif

/*
 * Benchmark for LIB9511 and LIBF.
 * Each float is printed as a long after scaling by 1000000.
 * Operands are variables so the chip path runs.
 * Build the LIBF baseline with -DBASE.
 * That build has no modf and no am9511.h names.
 */

long scale(x)
double x;
{
	return (long)(x * 1000000.0);
}

int main()
{
	double a, x, y;
	long n;
	unsigned long u;
#ifndef BASE
	double ip, fp;
#endif

	printf("start\n");
	x = 1.25;
	y = 2.5;
	a = x + y;
	printf("add %ld\n", scale(a));
	x = 1.0;
	y = 0.5;
	a = x - y;
	printf("sub %ld\n", scale(a));
	a = x - x;
	printf("sub0 %ld\n", scale(a));
	x = 0.0;
	y = 2.5;
	a = x + y;
	printf("add0 %ld\n", scale(a));
	a = y + x;
	printf("add0r %ld\n", scale(a));
	a = x - y;
	printf("subl %ld\n", scale(a));
	a = y - x;
	printf("subr %ld\n", scale(a));
	x = 1.5;
	y = 2.0;
	a = x * y;
	printf("mul %ld\n", scale(a));
	x = 10.0;
	y = 4.0;
	a = x / y;
	printf("div %ld\n", scale(a));
	x = -2.5;
	y = 1.0;
	a = x + y;
	printf("neg %ld\n", scale(a));
	x = 1.0;
	y = 3.0;
	a = x / y;
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
	printf("fl15 %ld\n", scale(floor(1.5)));
	printf("flm15 %ld\n", scale(floor(-1.5)));
	printf("flm05 %ld\n", scale(floor(-0.5)));
	printf("ce15 %ld\n", scale(ceil(1.5)));
	printf("cem15 %ld\n", scale(ceil(-1.5)));
	printf("ce05 %ld\n", scale(ceil(0.5)));
	x = 65536.0;
	y = 65536.0;
	x = x * y;
	printf("fl32 %ld\n", (long)(floor(x) / y));
	x = -65536.0;
	x = x * y;
	printf("flm32 %ld\n", (long)(floor(x) / y));

	n = (long)(double)12345L;
	printf("ltof %ld\n", n);
	n = (long)(double)-12345L;
	printf("lneg %ld\n", n);
	n = (long)(double)-2;
	printf("itof %ld\n", n);
	n = (long)(double)200;
	printf("itou %ld\n", n);
	u = 0x7fffffffL;
	u = u + 1;
	n = (long)((double)u / 65536.0);
	printf("u31 %ld\n", n);

	x = 0.5;
	printf("sin %ld\n", scale(sin(x)));
	printf("cos %ld\n", scale(cos(x)));
	printf("tan %ld\n", scale(tan(x)));
	printf("asin %ld\n", scale(asin(x)));
	printf("acos %ld\n", scale(acos(x)));
	printf("atan %ld\n", scale(atan(x)));
	x = -0.5;
	printf("sinn %ld\n", scale(sin(x)));
	x = 2.0;
	printf("ln2 %ld\n", scale(log(x)));
	printf("lg10 %ld\n", scale(log10(x)));
	x = 1.0;
	printf("exp1 %ld\n", scale(exp(x)));
	x = 0.0;
	printf("exp0 %ld\n", scale(exp(x)));
	x = 2.0;
	y = 3.0;
	printf("pow23 %ld\n", scale(pow(x, y)));
	y = 0.0;
	printf("pow20 %ld\n", scale(pow(x, y)));
	x = 0.0;
	y = 2.0;
	printf("pow02 %ld\n", scale(pow(x, y)));
	y = 0.0;
	printf("pow00 %ld\n", scale(pow(x, y)));
	x = -2.0;
	y = 3.0;
	printf("powm23 %ld\n", scale(pow(x, y)));
	y = 2.0;
	printf("powm22 %ld\n", scale(pow(x, y)));
	y = 0.5;
	printf("powm2h %ld\n", scale(pow(x, y)));
	y = -1.0;
	printf("powm2n %ld\n", scale(pow(x, y)));
	x = -1.0;
	y = 2048.0;
	y = y * 4096.0;
	y = y + 1.0;
	printf("odd23 %ld\n", (long)y);
	printf("powodd %ld\n", scale(pow(x, y)));
	y = 2048.0;
	y = y * 4096.0;
	printf("even23 %ld\n", (long)y);
	printf("poweven %ld\n", scale(pow(x, y)));
	x = -1.0;
	printf("sqrtm %ld\n", scale(sqrt(x)));
	printf("logm %ld\n", scale(log(x)));
	x = 1.0;
	y = 0.0;
	a = x / y;
	printf("div0 %ld\n", scale(a));
	x = 1e10;
	a = x * x;
	printf("big %ld\n", scale(a));
	x = 1.0;
	y = 1.0;
	printf("atan21 %ld\n", scale(atan2(x, y)));

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
	printf("pi %ld\n", scale(pi()));
	x = 8.0;
	printf("log28 %ld\n", scale(log2(x)));
	x = 3.0;
	printf("exp23 %ld\n", scale(exp2(x)));
	x = 2.0;
	printf("exp102 %ld\n", scale(exp10(x)));
	x = 65536.0;
	x = x * x;
	x = x * 1024.0;
	x = x * 1024.0;
	x = x * 1024.0;
	printf("e10big %ld\n", (long)exp10(x));
	x = -1.0;
	printf("log2m %ld\n", scale(log2(x)));
#endif
	x = 1.25;
	y = 2.5;
	a = x + y;
	printf("tail %ld\n", scale(a));
	printf("done\n");
	return 0;
}
