/*------------------------------------------------------------------------*\
 | memset()
 |
 |	This is the original Hi-Tech code for memset():
 |
 |		memset(p, n, c)
 |		register char *	p;
 |		register int	n;
 |		char		c;
 |		{
 |			while(n--)
 |				*p++ = c;
 |		}
 |
 |	The declaration of memset() in string.h is:
 |
 |		void *memset(void *, int, size_t);
 |
 |	which is correct but which does not agree with the implementation.
 |	The problem with the Hi-Tech code is that the parameters are in the
 |	wrong order and that causes serious problems.
 |
 |	Herewith an implementation which matches the declaration and which
 |	conforms to the standard definition for memset().
 |
 |	Jon Saxton
 |	September 2012
\*-------------------------------------------------------------------------*/

#include <string.h>

void *memset(void *mem, int fill, size_t bytes)
{
	unsigned char
		*v = (unsigned char *)mem;
	while (bytes--)
		*v++ = fill;
	return mem;
}
 
 
