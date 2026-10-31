/*
 *	int stat(char * s, struct stat * b);
 *
 *	Fills  in the supplied struct stat with modification and
 *	access time and file size.
 */

#include	<stat.h>
#include	<time.h>
extern int errno;
extern int _flip(char *, char *, unsigned int);


struct fib /* MSX-DOS2 file info block */
{
  char magic; /* 0xff */
  char name[13];
  unsigned char attr; 
  unsigned short mtime;
  unsigned short mdate;
  short cluster;
  long size;
  char drive;
  char pad[64-26];
};

#define MAXLEN 67

static time_t seconds(struct tm * tp);

int
stat(register char * s, register struct stat * b)
{
  /* changes in local variables must be reflected in asm-code */
  struct tm td;
  struct fib info;
  char name[MAXLEN];  

  if (! _flip(name,s,MAXLEN)) return -1;
  else ;
#asm
  global _errno, bDos
  push ix
  pop de
  push de
  ld ix, -82 ; offset of info in frame
  add ix,de ; ix= (&info) 
  ld hl,-149 ; offset of name[] in frame
  add hl,de
  ex de,hl ; de=name
  ld bc, 01640h ; 40: _FFIRST, 16: hidden,system,directory
  call bDos
  pop ix
  jr z,1f
  ld (_errno),a
  xor a
  ld (_errno+1),a
  ld hl,-1
  jp cret
1:
#endasm

  b->st_mode = (info.attr & 0x10)?S_IREAD|S_IEXEC|S_IFDIR:S_IREAD|S_IFREG;
  if ((info.attr & 1)==0) b->st_mode |= S_IWRITE;
  if (info.attr & 2) b->st_mode |= S_HIDDEN;
  if (info.attr & 4) b->st_mode |= S_SYSTEM;
  if (info.attr & 0x20) b->st_mode |= S_ARCHIVE;
  
  td.tm_sec = (info.mtime & 0x1f) *2;
  td.tm_min = (info.mtime >> 5) & 0x3f;
  td.tm_hour= (info.mtime >> 11) & 0x1f;
  td.tm_mday=  info.mdate & 0x1f;
  td.tm_mon = (info.mdate >> 5) & 0xf;
  td.tm_year=((info.mdate >> 9) & 0x7f) + 1980;
  
  b->st_mtime=b->st_atime=seconds(& td);
  b->st_size=info.size;
  
  return 0;
}

/* computes seconds past since 1/1/1970 (ignoring leap seconds) */
static time_t seconds(struct tm * tp)
{
  int yr;
  unsigned long res;
  /* som[n] = number of days in first n months of non-leap year */
  static int som[12]={   0, 31, 59, 90,120,151,181,212,243,273,304,334 };

  res = (tp->tm_year-1970)*365 + (tp->tm_year-1969)/4; 
  res+= som[tp->tm_mon-1] + tp->tm_mday-1;
  if ((yr%4==0) && (tp->tm_mon > 2)) res++; /* days past since 1/1/1970 */
  /* leap year heuristic is wrong in 2100, > 2^{32} sec after 1/1/1970 */

  return ((res*24l + tp->tm_hour)*60l + tp->tm_min)*60l + tp->tm_sec;
}
