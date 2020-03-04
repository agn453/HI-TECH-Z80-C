#include	"cpm.h"

long
lseek(fd, offs, whence)
uchar	fd, whence;
long	offs;
{
	register struct fcb *	fc;
	long			pos;

	if(fd >= MAXFILE)
		return -1;
	fc = &_fcb[fd];
	switch(whence) {

	default:
		pos = offs;
		break;

	case 1:
		pos = fc->rwp + offs;
		break;

	case 2:
		if (fc->rwp > fc->fsize)
			pos = fc->rwp   + offs;
		else	pos = fc->fsize + offs;
		break;
	}
	if(pos >= 0) {
		fc->rwp = pos;
		return fc->rwp;
	}
	return -1;
}
