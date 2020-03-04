#include    "cpm.h"

extern char _piped; /* PIPEMGR loaded? */

write(uchar fd, char *buf, ushort nbytes)
{
    register struct fcb *fc;
    uchar   size, offs, luid;
    short   c;
    ushort  count;
    char    RSXPB[4];
    char    buffer[SECSIZE];

    if (fd >= MAXFILE)
        return -1;
    fc = &_fcb[fd];
    offs = CPMWCON;
    RSXPB[0]=0x7D;
    RSXPB[1]=0;
    RSXPB[3]=0;
    count = nbytes;
    switch (fc->use)
    {
    case U_PUN:
        while (nbytes--)
        {
            _sigchk();
            bdos(CPMWPUN, *buf++);
        }
        return count;

    case U_LST:
        offs = CPMWLST;

    case U_CON:
        while (nbytes--)
        {
            _sigchk();
            c = *buf++;
            bdos(offs, c);
        }
        return count;

    case U_ERR:
        RSXPB[0]=0x7A;
    case U_RSX:
        while (nbytes--)
        {
            _sigchk();
            RSXPB[2]= *buf++;
            if (_piped)
                bdos(CPMRSX,RSXPB);
            else
                bdos(CPMWCON,RSXPB[2]);
        }
        return count;   
    case U_WRITE:
    case U_RDWR:
        luid = getuid();
        while (nbytes)
        {
            _sigchk();
            setuid(fc->uid);
            offs = fc->rwp%SECSIZE;
            if ((size = SECSIZE - offs) > nbytes)
                size = nbytes;
            _putrno(fc->ranrec, fc->rwp/SECSIZE);
            if (size == SECSIZE)
            {
                bdos(CPMSDMA, buf);
#ifdef  LARGE_MODEL
                bdos(CPMDSEG, (int)((long)buf >> 16));  /* set DMA segment */
#endif
            }
            else
            {
                bdos(CPMSDMA, buffer);
#ifdef  LARGE_MODEL
                bdos(CPMDSEG, (int)((long)buffer >> 16));   /* set DMA segment */
#endif
                buffer[0] = CPMETX;
                bmove(buffer, buffer+1, SECSIZE-1);
                bdos(CPMRRAN, fc);
                bmove(buf, buffer+offs, size);
            }
            if (bdos(CPMWRAN, fc))
                break;
            buf += size;
            fc->rwp += size;
            if (fc->fsize < fc->rwp)
                fc->fsize = fc->rwp;
            nbytes -= size;
            setuid(luid);
        }
        setuid(luid);
        return count-nbytes;

    default:
        return -1;
    }
}
