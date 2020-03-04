#include    "cpm.h"

extern int errno;

unlink(char *name)
{
    struct fcb  fc;
    uchar       luid;
    short       retval;

    if (setfcb(&fc, name))
        return -1;
    luid = getuid();
    setuid(fc.uid);
    retval = bdos(CPMDEL, &fc);
    if ((retval & 0xFF) == 0xFF && errno == 16)
        errno = 7; /* file not found */
    setuid(luid);
    return retval;
}
