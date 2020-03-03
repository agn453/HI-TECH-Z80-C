#include	"cpm.h"

write(fd, buf, nbytes)
uchar	fd;
ushort	nbytes;
char *	buf;
{
	register struct fcb *	fc;
	uchar	size, offs, luid;
	short	c;
	ushort	count;
	char	buffer[SECSIZE];

	if(fd >= MAXFILE)
		return -1;
	fc = &_fcb[fd];
	offs = CPMWCON;
	count = nbytes;
	switch(fc->use) {

	case U_PUN:
		while(nbytes--) {
			_sigchk();
			bdos(CPMWPUN, *buf++);
		}
		return count;

	case U_LST:
		offs = CPMWLST;
	case U_CON:
		while(nbytes--) {
			_sigchk();
			c = *buf++;
			bdos(offs, c);
		}
		return count;

	case U_WRITE:
	case U_RDWR:
		luid = getuid();
		while(nbytes) {
			_sigchk();
			setuid(fc->uid);
			offs = fc->rwp%SECSIZE;
			if((size = SECSIZE - offs) > nbytes)
				size = nbytes;
			_putrno(fc->ranrec, fc->rwp/SECSIZE);
			if(size == SECSIZE) {
				bdos(CPMSDMA, buf);
#ifdef	LARGE_MODEL
				bdos(CPMDSEG, (int)((long)buf >> 16));	/* set DMA segment */
#endif
			} else {
				bdos(CPMSDMA, buffer);
#ifdef	LARGE_MODEL
				bdos(CPMDSEG, (int)((long)buffer >> 16));	/* set DMA segment */
#endif
				buffer[0] = CPMETX;
				bmove(buffer, buffer+1, SECSIZE-1);
				bdos(CPMRRAN, fc);
				bmove(buf, buffer+offs, size);
			}
			if(bdos(CPMWRAN, fc))
				break;
			buf += size;
			fc->rwp += size;
			nbytes -= size;
			setuid(luid);
		}
		setuid(luid);
		return count-nbytes;

	default:
		return -1;
	}
}
