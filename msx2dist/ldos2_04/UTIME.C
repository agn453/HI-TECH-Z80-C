/* utime.c */

#include <time.h>
#include <stat.h>

extern int _flip(char*, char*, int); 
extern int _utime(char*, int, int);

#define BUFLEN 67

int utime(char * name, struct utimbuf *tb)
{
  register struct tm *tp;
  int fat_date, fat_time;
  char buf[BUFLEN];

  if (_flip(buf, name, BUFLEN)) { /* '/' to '\\' */ 
    tp=gmtime(tb->mod_time);
    fat_date= (tp->tm_year-80)<<9 + tp->tm_mon<<5 + tp->tm_mday;
    fat_time=  tp->tm_hour << 11 + tp->tm_min << 5 + tp->tm_sec >>1;
    return _utime(buf, fat_date, fat_time);
  }
  return -1;
}
