#include	<cpm.h>
#include	<stat.h>

chmod(name, mode)
register char *	name;
{
	struct fcb 	fc;
	register short	luid;

	if(!setfcb(&fc, name)) {
		luid = getuid();
		setuid(fc.uid);
		if(!(mode & S_IWRITE))
			fc.ft[0] |= 0x80;
		if(mode & S_SYSTEM)
			fc.ft[1] |= 0x80;
		if(mode & S_ARCHIVE)
			fc.ft[2] |= 0x80;
		mode = bdos(CPMSATT, &fc);
		setuid(luid);
		return mode;
	}
	return -1;
}
