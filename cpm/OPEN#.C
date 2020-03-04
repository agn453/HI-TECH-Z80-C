#include	"cpm.h"

open(name, mode)
char *	name;
{
	register struct fcb *	fc;
	uchar			luid;

	if(++mode > U_RDWR)
		mode = U_RDWR;
	if(!(fc = getfcb()))
		return -1;
	if(!setfcb(fc, name)) {
		if(mode == U_READ && bdos(CPMVERS) >= 0x30)
			fc->name[5] |= 0x80;	/* read-only mode */
		luid = getuid();
		setuid(fc->uid);
		if(bdos(CPMOPN, fc) == -1) {
			putfcb(fc);
			setuid(luid);
			return -1;
		}
		setuid(luid);
		fc->use = mode;
	}
	return fc - _fcb;
}
