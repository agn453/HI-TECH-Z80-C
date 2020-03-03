#include	"cpm.h"
#include	<string.h>

/*
 *	char *	fcbname(i);
 *	short	i;
 *
 *	Returns a character string which is the name of the file currently
 *	open on file descriptor i. The name is extracted from the corresponding
 *	fcb.
 */

char *
fcbname(i)
short	i;
{
	register struct fcb *	fc;
	static char		abuf[20];
	register char *		cp, * xp;


	fc = &_fcb[i];
	switch(fc->use) {

	case U_CON:
		return "CON:";
	case U_RDR:
		return "RDR:";
	case U_PUN:
		return "PUN:";
	case U_LST:
		return "LST:";

	case U_READ:
	case U_WRITE:
	case U_RDWR:
		cp = abuf;
		sprintf(cp, "%d:", fc->uid);
		cp += strlen(cp);
		if(fc->dr) {
			*cp++ = 'A' + fc->dr - 1;
			*cp++ = ':';
		}
		for(xp = fc->name ; *xp > ' ' && xp < fc->name+sizeof fc->name ;)
			*cp++ = *xp++;
		*cp++ = '.';
		for(xp = fc->ft ; *xp > ' ' && xp < fc->ft+sizeof fc->ft ;)
			*cp++ = *xp++;
		*cp = 0;
		return abuf;

	default:
		return (char *)0;
	}
}
