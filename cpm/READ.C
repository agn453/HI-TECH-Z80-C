#include	"cpm.h"

read(fd, buf, nbytes)
uchar	fd;
ushort	nbytes;
char *	buf;
{
	register struct fcb *	fc;
	uchar	size, offs, luid;
	ushort	cnt;
	char	buffer[SECSIZE+2];

	cnt = 0;
	if(fd >= MAXFILE)
		return -1;
	fc = &_fcb[fd];
	switch(fc->use) {

	case U_RDR:
		cnt = nbytes;
		while(nbytes) {
			nbytes--;
			if((*buf++ = (bdos(CPMRRDR) & 0x7f)) == '\n')
				break;
		}
		return cnt - nbytes;

	case U_CON:
		if(nbytes > SECSIZE)
			nbytes = SECSIZE;
		buffer[0] = nbytes;
		bdos(CPMRCOB, buffer);
		cnt = (uchar)buffer[1];
		if(cnt < nbytes) {
			bdos(CPMWCON, '\n');
			buffer[cnt+2] = '\n';
			cnt++;
		}
		bmove(&buffer[2], buf, cnt);
		return cnt;

	case U_READ:
	case U_RDWR:
		luid = getuid();
		cnt = nbytes;
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
				if(bdos(CPMRRAN, fc))
					break;
			} else {
				bdos(CPMSDMA, buffer);
#ifdef	LARGE_MODEL
				bdos(CPMDSEG, (int)((long)buffer >> 16));	/* set DMA segment */
#endif
				if(bdos(CPMRRAN, fc))
					break;
				bmove(buffer+offs, buf, size);
			}
			buf += size;
			fc->rwp += size;
			nbytes -= size;
			setuid(luid);
		}
		setuid(luid);
		return cnt - nbytes;

	default:
		return -1;
	}
}
