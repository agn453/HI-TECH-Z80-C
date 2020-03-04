#include    "cpm.h"

extern int errno;

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
    if ((d = bdos(CPMFFST, fc) & 0xFF) < 4)   /* Account for CP/M3 bytewise */
    {
        d = (buf[13 + (d << 5)] & 0x7F);  /* file sizes */
/*      if (d)               // Byte count represents UNUSED bytes in last
            d = 0x80 - d;    // sector ... Changed 9 Apr 2014 jrs */
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
            fc->name[5] |= 0x80;    /* read-only mode */
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
