#include	"cpm.h"

unlink(name)
char *	name;
{
	struct fcb	fc;
	uchar		luid;
	short		retval;

	if(setfcb(&fc, name))
		return 0;
	luid = getuid();
	setuid(fc.uid);
	retval = bdos(CPMDEL, &fc);
	setuid(luid);
	return retval;
}
