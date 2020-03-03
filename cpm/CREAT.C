#include	"cpm.h"

creat(name)
char *	name;
{
	register struct fcb *	fc;
	uchar			luid;

	if(!(fc = getfcb()))
		return -1;
	luid = getuid();
	if(!setfcb(fc, name)) {
		unlink(name);
		setuid(fc->uid);
		if(bdos(CPMMAKE, fc) == -1) {
			setuid(luid);
			fc->use = 0;
			return -1;
		}
		setuid(luid);
		fc->use = U_WRITE;
	}
#if	0
	fc->dm[0] = 0;
	bmove((char *)fc->dm, (char *)&fc->dm[1], sizeof fc->dm - 1);
#endif	0
	return fc - _fcb;
}
