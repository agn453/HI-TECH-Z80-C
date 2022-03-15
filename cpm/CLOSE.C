#include    <cpm.h>
/*
    This is a modified version of close.c to support exact file sizes.

    Unfortunately there are two common conventions used to record the exact size in CP/M.
    1) Record the bytes used in the last sector - used by DOSPLUS
    2) Record the bytes unused in the last sector - used by ISX for ISIS emulation

    A previous version of this file only supported convention 2.

    To support both of these variants an environment variable EXACT is used.
    Hi-Tech C supports this through the use of a file ENVIRON
    The values for EXACT are noted below, but in reality only the first
    character is checked.
    EXACT=DOSPLUS     This uses convention 1 above
    EXACT=ISX         This uses convention 2 above
    if EXACT is not defined or doesn't start with a 'D' or 'I', exact file size
    isn't used.

    Note you need to take extra care in setting EXACT if you are using an emulator
    that can access host files and truncates files to match the exact size.
    Using the wrong mode will mean that the value this function passes to the emulator
    will be wrong and could result in data losss. If in doubt I recommend exact size isn't used.

    For the ZXCC emulator, using EXACT=DOSPLUS is safe.
*/

static char exact = 0;  /* 0 = not determined, 'C' = not used (old CP/M),
                           'D' = DOSPLUS mode,  'I' = ISX mode
                        */

char *getenv(char *);
int toupper(int );

int close(uchar fd)
{
    register struct fcb *fc;
    uchar       luid;
    char *env;

    if (fd >= MAXFILE)
        return -1;
    fc = &_fcb[fd];
    luid = getuid();
    setuid(fc->uid);
    if (fc->use == U_WRITE || fc->use == U_RDWR
                           || bdos(CPMVERS)&(MPM|CCPM) && fc->use == U_READ)
        bdos(CPMCLS, fc);
    if (!exact) { /* get the exact size mode, checking only once */
        if ((env = getenv("EXACT")))
            exact = toupper(*env);
        if (exact != 'I' && exact != 'D')
            exact = 'C';
    }
    if (exact != 'C') { /* skip if old CP/M mode */
        fc->nr = (exact == 'D' ? fc->fsize : -fc->fsize) & 0x7f;  /* Set exact file size */
        fc->name[5] |= 0x80;
        if (fc->use == U_WRITE || fc->use == U_RDWR)
            bdos(CPMSATT, fc);
    }
    fc->use = 0;
    setuid(luid);
    return 0;
}
