/* Convert structure to time value */

#include <time.h>

/* leap year calculator expects year argument as years offset from 1900 */
#define LEAP_YEAR(Y)  ( ((1900+(Y))>0) && !((1900+(Y))%4) && ( ((1900+(Y))%100) || !((1900+(Y))%400) ) )

#define SECS_PER_DAY  (86400l)
#define SECS_PER_HOUR (3600l)
#define SECS_PER_MIN  (60l)

static unsigned char monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
 
time_t mktime(struct tm *ptm){   
  
   int i;
   time_t seconds;

   /* seconds from 1970 till 1 jan 00:00:00 of the given year */
   seconds= (ptm->tm_year-70)*(SECS_PER_DAY * 365);
   for (i = 70; i < ptm->tm_year; i++) {
      if (LEAP_YEAR(i)) {
         seconds += SECS_PER_DAY;   /* add extra days for leap years */
      }
   }
  
   /* add days for this year */
   for (i = 0; i < ptm->tm_mon; i++) {
      if ( (i == 1) && LEAP_YEAR(ptm->tm_year)) { 
         seconds += SECS_PER_DAY * 29;
      } else {
         seconds += SECS_PER_DAY * (int)monthDays[i];
      }
   }
   seconds += (ptm->tm_mday-1) * SECS_PER_DAY;
   seconds += ptm->tm_hour * SECS_PER_HOUR;
   seconds += ptm->tm_min * SECS_PER_MIN;
   seconds += ptm->tm_sec;
   seconds += time_zone * SECS_PER_MIN;
   return (time_t)seconds; 
}
