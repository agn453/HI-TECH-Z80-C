#include	<cpm.h>
#include	<ctype.h>

/*
 *	Syntax of CP/M file names is:
 *
 *	[[0-9]+:][[a-pA-P]:]name[.ext]
 *
 *	E.g.
 *		file.txt
 *		a:file
 *		10:b:READ-ME.doc
 */


#define	DNAMLEN	4	/* length of device names */

static char	dnames[][DNAMLEN] =
{
	"CON:",
	"RDR:",
	"PUN:",
	"LST:",
};

#define	NDNAMES	4

extern int	atoi(char *);

static char
upcase(c)
char	c;
{
	if(islower(c))
		return toupper(c);
	return c;
}

struct fcb *
getfcb()
{
	register struct fcb *	fp;

	for(fp = _fcb ; fp < &_fcb[MAXFILE] ; fp++)
		if(!fp->use) {
			fp->use = U_READ;
			fp->rwp = 0;
			return fp;
		}
	return (struct fcb *)0;
}

putfcb(fc)
register struct fcb *	fc;
{
	fc->use = 0;
}

uchar
setfcb(fc, name)
register char *		name;
struct fcb *	fc;
{
	uchar			i,j;

	while(isspace(*name))
		name++;
	for(i = 0 ; i < NDNAMES ; i++)
		for(j = 0 ; ;)
			if(dnames[i][j] != upcase(name[j]))
				break;
			else if(++j == DNAMLEN) {
				fc->use = i+U_CON;
				return 1;
			}
	fc_parse(fc, name);
	return 0;
}

fc_parse(fc, name)
register char *	name;
register struct fcb *	fc;
{
	register char *	cp;
	char		c;

	fc->dr = 0;
	fc->uid = getuid();
	cp = name;
	while(isdigit(*cp))
		cp++;
	if(name != cp && *cp == ':') {
		fc->uid = atoi(name);
		name = cp+1;
	}
	if(*name && name[1] == ':') {
		fc->dr = upcase(*name) - 'A' + 1;
		name += 2;
	}
	cp = fc->name;
	while(*name != '.' && *name != '*' && *name > ' ' && cp < &fc->name[8])
		*cp++ = upcase(*name++);
	if(*name == '*')
		c = '?';
	else
		c = ' ';
	while(cp < &fc->name[8])
		*cp++ = c;
	while(*name && *name++ != '.')
		continue;
	while(*name > ' ' && *name != '*' && cp < &fc->ft[3])
		*cp++ = upcase(*name++);
	if(*name == '*')
		c = '?';
	else
		c = ' ';
	while(cp < &fc->ft[3])
		*cp++ = c;
	fc->ex = fc->nr = 0;
}
