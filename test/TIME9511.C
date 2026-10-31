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

/*
 * Time 20000 add and subtract pairs.
 * The operands are variables so the compiler keeps the calls.
 * The scaled result is 1250000 when the value stays 1.25.
 */

main()
{
	double a, b, c;
	unsigned i;

	printf("start\n");
	a = 1.25;
	b = 2.5;
	c = 2.5;
	for (i = 0; i < 20000; i++)
		a = a + b - c;
	printf("timed %ld\n", (long)(a * 1000000.0));
	printf("done\n");
	return 0;
}
