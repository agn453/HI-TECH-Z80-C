#include	<stdio.h>

extern long	lseek(int, long, int), ftell(FILE *);

fseek(f, offs, ptr)
register FILE *	f;
long		offs;
int		ptr;
{
  return lseek(fileno(f), offs, ptr);
}

long
ftell(FILE * f)
{
   return lseek(fileno(f),0L,1);
}

