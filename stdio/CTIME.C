/*
 *	Ctime for HI-TECH C - no daylight saving
 */

#include	<time.h>

unsigned char	monlen[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static
put2d(cp, i, cl, ct)
char *		cp, cl, ct;
unsigned	i;
{
	*cp = ct;
	*--cp = i%10 + '0';
	if(i /= 10)
		*--cp = i + '0';
	else
		*--cp = cl;
}

static
dylen(yr)
unsigned	yr;
{
	if(yr%4)
		return(365);
	return(366);
}

struct tm *
localtime(tp)
time_t *	tp;
{
	time_t	t;

	t = *tp - time_zone*60L;
	return gmtime(&t);
}

struct tm *
gmtime(tp)
time_t *	tp;
{
	unsigned	day, mon, yr, cumday;
	time_t		t;
	static struct tm	tim;

	t = *tp;
	tim.tm_sec = t % 60L;
	t /= 60L;
	tim.tm_min = t % 60L;
	t /= 60L;
	tim.tm_hour = t % 24L;
	day = t / 24L;
	tim.tm_wday = (day + 4) % 7;
	yr = 70;
	cumday = 0;
	while((cumday += dylen(yr)) <= day)
		yr++;
	tim.tm_year = yr;
	cumday -= dylen(yr);
	day -= cumday;
	tim.tm_yday = day;
	cumday = 0;
	mon = 0;
	if((yr % 4) == 0)
		monlen[1] = 29;
	else
		monlen[1] = 28;
	while((cumday += monlen[mon]) <= day)
		mon++;
	cumday -= monlen[mon];
	day -= cumday;
	tim.tm_mday = day + 1;
	tim.tm_mon = mon;
	return &tim;
}

char *
asctime(tim)
register struct tm *	tim;
{
	register char *	s, * cp;
	short		i;
	static char	buf[27];

	s = &"SunMonTueWedThuFriSat"[tim->tm_wday*3];
	i = 3;
	cp = buf;
	do
		*cp++ = *s++;
	while(--i);
	*cp++ = ' ';
	s = &"JanFebMarAprMayJunJulAugSepOctNovDec"[tim->tm_mon*3];
	i = 3;
	do
		*cp++ = *s++;
	while(--i);
	buf[7] = ' ';
	put2d(buf+10, tim->tm_mday, ' ', ' ');
	put2d(buf+13, tim->tm_hour, '0', ':');
	put2d(buf+16, tim->tm_min, '0', ':');
	put2d(buf+19, tim->tm_sec, '0', ' ');
	put2d(buf+22, tim->tm_year/100 + 19, ' ', 0);
	put2d(buf+24, tim->tm_year%100, '0', '\n');
	return(buf);
}

char *
ctime(tp)
time_t *	tp;
{
	return asctime(localtime(tp));
}
