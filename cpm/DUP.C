#include	"cpm.h"

dup(fd)
uchar	fd;
{
	register struct fcb *	fp;

	if(_fcb[fd].use && (fp = getfcb())) {
		*fp = _fcb[fd];
		return fp - _fcb;
	}
	return -1;
}
