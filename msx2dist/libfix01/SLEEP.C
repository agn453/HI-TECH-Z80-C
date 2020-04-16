/*
 * Sleep function for Hi-Tech C
 * PGN 12/93
 */

#include <time.h>

void sleep(seconds)
unsigned int seconds;
{
	long t0;
	t0 = (long)time((long *)0)+seconds;
	while (time((long *)0)<=t0);		/* wait seconds */
}

