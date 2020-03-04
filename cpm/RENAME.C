#include    "cpm.h"

int rename(char *n1, char *n2)
{
    struct fcb  fc[2];

    unlink(n2);
    if (!(setfcb(&fc[0], n1) || setfcb((struct fcb *)(fc[0].dm), n2))
      && (bdos(CPMREN, &fc[0]) & 0xFF) >= 0)
        return 0;
    return -1;
}
