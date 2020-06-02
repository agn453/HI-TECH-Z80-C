/*
 *  time() for CP/M-86 type machines
 */

#include    <cpm.h>
#include    <time.h>

struct tod
{
    short   days;
    char    hours;
    char    mins;
    char    secs;
};

extern time_t   convtime();

time_t time(time_t *tp)
{
    struct tod  tod;
    time_t      t;

    tod.hours = tod.mins = tod.secs = tod.days = 0;
    tod.secs = bdos(CPMGDAT, &tod);
    t = convtime(&tod);
    if (tp)
        *tp = t;
    return t;
}
