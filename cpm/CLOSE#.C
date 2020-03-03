#include	"cpm.h"

close(fd)
uchar	fd;
{
	register struct fcb *	fc;
	uchar		luid;

	if(fd >= MAXFILE)
		return -1;
	fc = &_fcb[fd];
	luid = getuid();
	setuid(fc->uid);
	if(fc->use == U_WRITE || fc->use == U_RDWR || bdoshl(CPMVERS)&(MPM|CCPM) && fc->use == U_READ)
		bdos(CPMCLS, fc);
	fc->use = 0;
	setuid(luid);
	return 0;
}
