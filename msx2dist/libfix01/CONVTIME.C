/*
 * This routine converts the date and time in MS(X)DOS format to
 * Unix style date and time - seconds since 00:00:00 Jan 1 1970.
 * Pierre Gielen 10/93
 *
 * Public Domain for Hitech-C standard MSXDOS library
 */

#include <time.h>

long convtime(dostime,dosdate)
unsigned int dosdate,dostime;
{
	unsigned int cday = 0;
        unsigned int cnt,year,month;
	time_t t;
	static unsigned char dpermo[12] =
		     {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        year = ((dosdate >> 9) & (int)0x7f)+10;     /* bits 15..9  */
	month = ((dosdate >> 5) & (int)0xf)-1;	    /* bits  8..5  */

        for (cnt=0;cnt<year;cnt++)
	{
	  cday += 365;
	  if (((1970+cnt) % 4)==0)	/* leapyear */
	    ++cday;
	}
        if(((1970+year)%4)==0)          /* if current leapyear */
	  if(month>1)			/* if past february */
	    ++cday;
        for (cnt=0;cnt<month;cnt++)
	  cday += dpermo[cnt];		/* add days per month */

        t = (long)(cday + (dosdate & (int)0x1f)-1); /* days bits  4..0  */
        t *= 24;                                    /* date in past hours */
        t += ((dostime >> 11) & (int)0x1f);         /* hours, bits 15..11 */
	t *= 60;
        t += ((dostime >> 5) & (int)0x3f);          /* minutes, bits 10..5  */
	t *= 60;
        t += ((dostime << 1) & (int)0x3e);          /* seconds, bits  4..0  */

	return (long)t;
}

