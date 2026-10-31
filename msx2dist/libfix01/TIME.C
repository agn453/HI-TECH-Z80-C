/*
 * MSX system-dependant time(), timerset() and timeup() functions
 * for Hi-Tech C 3.10.
 * time() returns (long)time in UNIX format: seconds since 00:00:00,
 * 1/1/1970. Bug fixed for leap year test, 6/94.
 * timerset() returns a (long) timervalue.
 * timeup() checks if time for that value is up (only 1 second
 * resolution, use mtimerset() and mtimeup() macros in time.h for
 * 100ms resolution.
 * Public domain by Pierre Gielen.
 * The function msxtime() is moved to msxtime.as by Arnold Metselaar 1/06.
 */

#include	<time.h>

/* moved to msxtime.as: */
extern time_t msxtime();

time_t
time(tp)
time_t *	tp;
{
	time_t t;
	t = msxtime();
	if(tp)
	   *tp = t;
	return t;
}

time_t timerset(value)
long value;
{
	return (msxtime()+value);
}

int timeup(timer)
long timer;
{
	if(msxtime() < timer)
	  return(0);
	return(1);
}


