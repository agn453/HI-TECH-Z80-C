/*
**  strtok() -- public domain by Ray Gardner, modified by Bob Stout
**              not modified for Hi-Tech C by Pierre Gielen :-)
**              comment corrected by Arnold Metselaar
**
**   You pass this function a string to parse, a buffer to receive the
**   "token" that gets scanned, the length of the buffer, and a string of
**   "break" characters that stop the scan.  It will copy the string into
**   the buffer up to any of the break characters, or until the buffer is
**   full, and will always leave the buffer null-terminated.  It will
**   return a pointer to the first character from s that was not copied to
**   tok.
*/

#include <stddef.h>

char *strtok(char *s, char *tok, size_t toklen, char *brk)
{
      char *lim, *b;

      if (!*s)
            return NULL;

      lim = tok + toklen - 1;
      while (*s && (tok<lim))
      {
            for (b=brk;*b;b++)
            {
                  if (*s==*b)
                  {
                        *tok=0;
                        return(s);
                  }
            }
            *tok++ = *s++;
      }
      *tok = 0;
      return(s);
}
