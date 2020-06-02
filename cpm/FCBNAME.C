#include    <cpm.h>
#include    <string.h>

/*
 *  char *  fcbname(i);
 *  short   i;
 *
 *  Returns a character string which is the name of the file currently
 *  open on file descriptor i. The name is extracted from the corresponding
 *  fcb.
 */

char *fcbname(short i)
{
    register struct fcb *fc;
    static char          abuf[20];
    register char       *cp, *xp;


    fc = &_fcb[i];
    switch(fc->use)
    {
    case U_CON:
        return "CON:";

    case U_RDR:
        if ((bdos(CPMVERS)&0xFF) < 0x30)
            return "RDR:";
        else
            return "AUXIN:";
    case U_PUN:
        if ((bdos(CPMVERS)&0xFF) < 0x30)
            return "PUN:";
        else
            return "AUXOUT:";
    case U_LST:
        return "LST:";

    case U_READ:
    case U_WRITE:
    case U_RDWR:
        cp = abuf;
        if (fc->dr)
            *cp++ = 'A' + fc->dr - 1;
        sprintf(cp, "%d:", fc->uid);
        cp += strlen(cp);
        for (xp = fc->name ; *xp > ' ' && xp < fc->name+sizeof fc->name;)
            *cp++ = (*xp++) & 0x7F;
        *cp++ = '.';
        for (xp = fc->ft ; *xp > ' ' && xp < fc->ft+sizeof fc->ft ;)
            *cp++ = (*xp++) & 0x7F;
        *cp = 0;
        return abuf;

	case U_RSX:
		return "RSX:";
	case U_ERR:
		return "ERR:";

    default:
        return (char *)0;
    }
}
