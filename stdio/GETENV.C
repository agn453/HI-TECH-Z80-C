#include    <stdio.h>
#include    <string.h>
#if CPM
#include    <cpm.h>
#endif

#define ENVFILE "0:A:ENVIRON"   /* Must be of the form 0:X: */

       char **    environ;
extern char *    sbrk();

static char envname[]=ENVFILE;

static char loadenv(char *name, char **avec, char *abuf, short *i,
                    short avsize, short absize)
{
    FILE *fp;
    register char *cp;

    if((fp = fopen(name, "r")) == NULL) return(0);

    while((*i) < avsize && fgets(abuf, absize, fp)) {
        cp = sbrk(strlen(abuf)+1);
        strcpy(cp, abuf);
        cp[strlen(cp)-1] = 0;
        avec[(*i)++] = cp;
    }
    fclose(fp);

    return(1);
}

char * getenv(char * s)
{
    register char **    xp;
    short            i;
    static char        setup;

    if(!setup) {
        short           av,ab;
        unsigned short  scbpb;
        char        drive;
        char *        avec[40];
        char        abuf[128];

        i = 0;

#if CPM   /*  CP/M environment simulation? */
        av=sizeof avec / sizeof avec[0];
        ab=sizeof abuf;
        if((bdoshl(CPMVERS) & 0x0FF0)==0x0030)    /* CP/M 3 */
        {
            for (scbpb=0x4C; scbpb<0x50; scbpb++)
            {
                drive=bdos(CPMSCB,&scbpb);
                if (drive & 0x20) 
                    break; /* End of chain */ 
                else 
                {
                    /* if default, get current drive 0..15 */
                    if (!drive) 
                        drive=bdos(CPMIDRV)+1; /**AGN fix*/
                    envname[2]=drive+'@';
                    if (loadenv(envname+2,avec,abuf,&i,av,ab)) 
                        break;
                    if (loadenv(envname,avec,abuf,&i,av,ab)) 
                        break;
                 }
            }
            if (i==0) 
                loadenv(envname+4,avec,abuf,&i,av,ab);
        }
        else    /* Not CP/M 3. Search current drive & user 0 */
        {
            envname[2]='0';
            if (!loadenv(envname+2,avec,abuf,&i,av,ab))
                   loadenv(envname+4,avec,abuf,&i,av,ab);
        }
#else
        loadenv(envname+4,avec,abuf,&i,av,ab);
#endif
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
