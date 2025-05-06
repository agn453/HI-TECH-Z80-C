#include    <cpm.h>
/*
    This is a modified version of close.c to support exact file sizes.

    Unfortunately there are two common conventions used to record the
    exact size in CP/M.

    1) Record the bytes used in the last sector - used by DOSPLUS
    2) Record the bytes unused in the last sector - used by ISX for
       ISIS emulation

    A previous version of this file only supported convention 2.

    To support both of these variants, a global variable _exact is
    set-up by the start-up module.

    The values for _exact are noted below

    'D' for DOSPLUS     This uses convention 1 above
    'I' for ISIS/ISX    This uses convention 2 above
    'C' for CP/M 2      Exact file size is not used

    Under CP/M 3 this defaults to 'D' DOSplus mode.  If you wish
    to use ISIS mode, you must set _exact to 'I' prior to using
    file I/O routines - e.g.

        extern char _exact;

        _exact = 'I';

    Note you need to take extra care with _exact if you are using an
    emulator that can access host files and truncates files to match
    the exact size.  Using the wrong mode will mean that the value
    this function passes to the emulator will be wrong and could
    result in data losss. If in doubt I recommend exact size isn't used.

    For the ZXCC emulator, using _exact as 'D' is safe.
*/

extern char _exact;  /* Exact file size hint for last sector
			'C' = not used (old CP/M),
                        'D' = DOSPLUS mode (count is USED bytes),
			'I' = ISX mode (count is UNUSED bytes)
                      */

char *getenv(char *);
int toupper(int );

int close(uchar fd)
{
    register struct fcb *fc;
    uchar       luid;

    if (fd >= MAXFILE)
        return -1;
    fc = &_fcb[fd];
    luid = getuid();
    setuid(fc->uid);
    if (fc->use == U_WRITE || fc->use == U_RDWR
                           || bdos(CPMVERS)&(MPM|CCPM) && fc->use == U_READ)
        bdos(CPMCLS, fc);
    if (_exact != 'C') { /* skip if old CP/M mode */
        fc->nr = (_exact == 'D' ? fc->fsize : -fc->fsize) & 0x7f; /* Set exact file size */
        fc->name[5] |= 0x80;
        if (fc->use == U_WRITE || fc->use == U_RDWR)
            bdos(CPMSATT, fc);
    }
    fc->use = 0;
    setuid(luid);
    return 0;
}
