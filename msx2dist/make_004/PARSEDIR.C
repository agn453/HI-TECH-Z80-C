#include <stdio.h>
#include <limits.h>
#include <stat.h>
#include "make.h"

static struct stat statbuf;

#define endoftime ULONG_MAX		/* when i'm old and wise */

/*
 * Get a file's creation date.
 */
int getdate(FILENODE *f)
{
  char fnm[65];
  strncpy(fnm,f->fname,65);

  if (!stat(fnm,&statbuf))
    f->fdate = statbuf.st_mtime;
  else {
    fprintf(stderr, "Can't get date for file '%s'\n", f->fname);
    f->fdate = endoftime;
  }
  return 0;
}


/*
 * laterdt - compare two dates.
 * Return -1, 0 or 1 if date1 < date2, date1 == date2, or date1 > date2
 */
int laterdt(DATE date1, DATE date2)
{
  if(date1 > date2)
    return 1;
  if (date1 < date2)
    return -1;
  return 0;
}

