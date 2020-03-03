#include	<stat.h>
#include	<cpm.h>
#include	<time.h>

/*
 *	int stat(char * s, struct stat * b);
 *
 *	Fills  in the supplied struct stat with modification and
 *	access time and file size.
 */

struct tod
{
	int	days;
	int	hoursmins;
	char	secs;
};

extern time_t convtime();

int
stat(register char * s, register struct stat * b)
{
	short		c;
	struct fcb	fc;
	struct tod	td;

	c = getuid();
	if(setfcb(&fc, s))
		return -1;		/* A device ! */
	setuid(fc.uid);
	if(!bdos(0x23, &fc) || bdoshl(CPMVERS) < 0x30) {	/* get size */
		b->st_size = fc.ranrec[0] + ((long)fc.ranrec[1] << 8) + ((long)fc.ranrec[2] << 16);
		b->st_size *= SECSIZE;
		if(fc.ft[0] & 0x80)
			b->st_mode = S_IREAD|S_IFREG;
		else
			b->st_mode = S_IREAD|S_IFREG|S_IWRITE;
		if(fc.ft[1] & 0x80)
			b->st_mode |= S_SYSTEM;
		if(fc.ft[2] & 0x80)
			b->st_mode |= S_ARCHIVE;
		td.secs = 0;
		td.days = td.hoursmins = 0;
		if(!bdos(0x66, &fc)) {
			td.days = ((int *)&fc)[24/sizeof(int)];
			td.hoursmins = ((int *)&fc)[26/sizeof(int)];
			b->st_atime = convtime(&td);
			td.secs = 0;
			td.days = ((int *)&fc)[28/sizeof(int)];
			td.hoursmins = ((int *)&fc)[30/sizeof(int)];
			b->st_mtime = convtime(&td);
		}
		setuid(c);
		return 0;
	}
	setuid(c);
	return -1;
}
