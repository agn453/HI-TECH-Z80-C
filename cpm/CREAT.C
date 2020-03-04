#include    "cpm.h"

extern int unlink(char *name);
extern int errno;

int creat(char *name)
{
    register struct fcb *   fc;
    uchar           luid;

    if (!(fc = getfcb()))
        return -1;
    luid = getuid();
    if (!setfcb(fc, name))
    {
        if (unlink(name) == -1 && errno > 16)
            return -1;
        setuid(fc->uid);
        if ((bdos(CPMMAKE, fc) & 0xFF) == 0xFF)
        {
            setuid(luid);
            fc->use = 0;
            return -1;
        }
        setuid(luid);
        fc->use = U_WRITE;
        fc->fsize = 0L;
    }
#if 0
    fc->dm[0] = 0;
    bmove((char *)fc->dm, (char *)&fc->dm[1], sizeof fc->dm - 1);
#endif  0
    return fc - _fcb;
}
