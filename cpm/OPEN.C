#include    "cpm.h"

extern int errno;

extern char _exact;  /* Exact file size hint for last sector
			'C' = not used (old CP/M),
                        'D' = DOSPLUS mode (count is USED bytes),
			'I' = ISX mode (count is UNUSED bytes)
                      */

long _fsize(uchar fd)
{
    register struct fcb *fc;
    long            tmp;
    uchar           d, luid, buf[SECSIZE];

    if (fd >= MAXFILE)
        return -1;
    fc = &_fcb[fd];
    luid = getuid();
    setuid(fc->uid);
    bdos(CPMCFS, fc);
/*
    tmp = (long)fc->ranrec[0] + ((long)fc->ranrec[1] << 8)
                              + ((long)fc->ranrec[2] << 16);
    tmp *= SECSIZE;

    A slightly more efficient but more obscure coding ... */
    tmp = (*(long *)(&fc->ranrec[-1]) & 0xFFFFFF00) / 2;

    bdos(CPMSDMA,buf);
    /* Account for CP/M3 bytewise file sizes in last sector */
    if ((d = bdos(CPMFFST, fc) & 0xFF) < 4)
    {
        d = (buf[13 + (d << 5)] & 0x7F); /* Count of UNUSED bytes */
        if ((_exact == 'D') && (d != 0)) /* DOS Plus has count of USED bytes */
            d = 0x80 - d;
        tmp -= d;
    }
    setuid(luid);
    return tmp;
}


int open(char *name, int mode)
{
    register struct fcb *fc;
    uchar           luid;

    if (++mode > U_RDWR)
        mode = U_RDWR;
    if (!(fc = getfcb()))
        return -1;
    if (!setfcb(fc, name))
   {
        if (mode == U_READ && (bdos(CPMVERS)&0x7F) >= 0x30)
            fc->name[5] |= (char) 0x80;    /* read-only mode */
        luid = getuid();
        setuid(fc->uid);
        if ((bdos(CPMOPN, fc) & 0xFF)  == 0xFF)
        {
            putfcb(fc);
            setuid(luid);
            if (errno == 16)
                errno = 7; /* File not found */
            return -1;
        }
        fc->fsize = _fsize(fc - _fcb);    /* Set file size */
        setuid(luid);
        fc->use = mode;
    }
    return fc - _fcb;
}
