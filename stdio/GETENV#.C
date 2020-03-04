#include	<stdio.h>
#include	<string.h>

#define	ENVFILE	"ENVIRON"

       char **	environ;
extern char *	sbrk();

char *
getenv(s)
char *	s;
{
	register char **	xp;
	register char *		cp;
	short			i;
	static char		setup;

	if(!setup) {
		FILE *	fp;
		char *	avec[40];
		char	abuf[128];

		i = 0;
		if(fp = fopen(ENVFILE, "r")) {
			while(i < sizeof avec/sizeof avec[0] && fgets(abuf, sizeof abuf, fp)) {
				cp = sbrk(strlen(abuf)+1);
				strcpy(cp, abuf);
				cp[strlen(cp)-1] = 0;
				avec[i++] = cp;
			}
			fclose(fp);
		}
		avec[i] = 0;
		xp = (char **)sbrk(i * sizeof avec[0]);
		memcpy(xp, avec, i * sizeof avec[0]);
		environ = xp;
		setup = 1;
	}
	i = strlen(s);
	for(xp = environ ; *xp ; xp++)
		if(strncmp(*xp, s, i) == 0 && (*xp)[i] == '=')
			return *xp + i+1;
	return (char *)0;
}
