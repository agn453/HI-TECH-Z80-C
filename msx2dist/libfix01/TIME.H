/* time.h voor hitech-c
 * aanpassing 01/06
 */


#ifndef _TIME

typedef	long	time_t;		/* for representing times in seconds */
struct tm {
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
};
#define	_TIME
#endif	_TIME

#ifndef	_STDDEF
typedef	int		ptrdiff_t;	/* result type of pointer difference */
typedef	unsigned	size_t;		/* type yielded by sizeof */
#define	_STDDEF
#define	offsetof(ty, mem)	((int)&(((ty *)0)->mem))
#endif	_STDDEF

#define CLK_TCK 50			  /* for 50Hz interrupt frequency */
#define JIFFY (*(unsigned short *)0xfc9e) /* clock tick counter */
#define jtimerset(s) (JIFFY=(0-(s * 5)))   /* set timer in 100ms slices */
#define jtimeup() (JIFFY==0)

extern int	time_zone;	/* minutes WESTWARD of Greenwich */
				/* this value defaults to 0 since with
				   operating systems like MS-DOS there is
				   no time zone information available */
extern time_t time(time_t *); /* seconds since 00:00:00 Jan 1 1970 */
extern char * asctime(struct tm *);   /* converts struct tm to ascii time */
extern char * ctime();	      /* current local time in ascii form */
extern struct tm * gmtime();	   /* Universal time */
extern struct tm * localtime();    /* local time */

extern time_t timerset(time_t);    /* set a timer in seconds */
extern int timeup(time_t);	   /* check if timer expired */
extern size_t strftime(char *s, size_t maxs, char *f, struct tm *t);

