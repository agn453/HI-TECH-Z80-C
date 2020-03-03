#include	"cpm.h"

long
_fsize(fd)
uchar	fd;
{
	register struct fcb *	fc;
	long			tmp;
	uchar			luid;

	if(fd >= MAXFILE)
		return -1;
	fc = &_fcb[fd];
	luid = getuid();
	setuid(fc->uid);
	bdos(CPMCFS, fc);
	setuid(luid);
	tmp = (long)fc->ranrec[0] + ((long)fc->ranrec[1] << 8) + ((long)fc->ranrec[2] << 16);
	tmp *= SECSIZE;
	if(tmp > fc->rwp)
		return tmp;
	return fc->rwp;
}

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
		pos = offs + _fsize(fd);
		break;
	}
	if(pos >= 0) {
		fc->rwp = pos;
		return fc->rwp;
	}
	return -1;
}
