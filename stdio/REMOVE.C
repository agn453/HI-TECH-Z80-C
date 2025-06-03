#include	<unixio.h>

remove(s)
char *	s;
{
	return unlink(s);
}
