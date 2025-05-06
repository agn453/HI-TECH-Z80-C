#include    <stat.h>
#include    <cpm.h>
#include    <time.h>

extern char _exact;  /* Exact file size hint for last sector
			'C' = not used (old CP/M),
                        'D' = DOSPLUS mode (count is USED bytes),
			'I' = ISX mode (count is UNUSED bytes)
                      */

/*
 *  int stat(char * s, struct stat * b);
 *
 *  Fills  in the supplied struct stat with modification and
 *  access time and file size.
 */

struct tod
{
    int days;
    int hoursmins;
    char    secs;
};

extern time_t convtime();

int stat(register char * s, register struct stat * b)
{
    short       c,d;
    struct fcb  fc;
    struct tod  td;
    uchar       buf[SECSIZE+1];    /* For bytewise lengths */

    c = getuid();
    if (setfcb(&fc, s))
        return -1;      /* A device ! */
    setuid(fc.uid);
    if (!bdos(CPMCFS, &fc) || bdos(CPMVERS) < 0x30)
    {
        /* get size */
        b->st_size = fc.ranrec[0]
                   + ((long)fc.ranrec[1] << 8)
                   + ((long)fc.ranrec[2] << 16);
        b->st_size *= SECSIZE;
        if (fc.ft[0] & 0x80)
            b->st_mode = S_IREAD|S_IFREG;
        else
            b->st_mode = S_IREAD|S_IFREG|S_IWRITE;
        if (fc.ft[1] & 0x80)
            b->st_mode |= S_SYSTEM;
        if (fc.ft[2] & 0x80)
            b->st_mode |= S_ARCHIVE;
        td.secs = 0;
        td.days = td.hoursmins = 0;
        bdos(CPMSDMA,buf);
        if ((d = bdos(CPMFFST,&fc)) < 4)
        {
            /* Account for CP/M3 bytewise file sizes */
            d = (buf[13 + (d << 5)] & 0x7F); /* count of UNUSED bytes */
            if (( _exact == 'D') && (d != 0)) /* DOS Plus has USED bytes */
                d=0x80-d;
            b->st_size-=d;
        }
	/* Corrected to account for directory code return [jrs 2014-04-20] */
        if ((bdos(CPMGFTS, &fc) & 0xFF) < 4) /* Get file date stamp */
        {
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
