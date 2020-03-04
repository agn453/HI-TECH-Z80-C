/*
 *	time() for CP/M-86 type machines
 */

#include	<time.h>
#include	<cpm.h>

struct tod
{
	short	days;
	char	hours;
	char	mins;
	char	secs;
};

#define	T_GET	0x69
extern time_t	convtime();

time_t
time(tp)
time_t *	tp;
{
	struct tod	tod;
	time_t		t;

	tod.hours = tod.mins = tod.secs = tod.days = 0;
	tod.secs = bdos(T_GET, &tod);
	t = convtime(&tod);
	if(tp)
		*tp = t;
	return t;
}
