#include    <cpm.h>
#include    <ctype.h>
#include    <sys.h>

/*
 *  Syntax of CP/M file names is:
 *
 *  [[0-9]+[:]][[a-pA-P]:]name[.ext] | [[a-pA-p][[0-9]+]:]name[.ext]
 *
 *  E.g.
 *      file.txt
 *      a:file
 *      10:b:READ-ME.doc
 */


#define DNAMLEN 4   /* length of device names */

static char dnames[][DNAMLEN] =
{
    "CON:",
    "RDR:",
    "PUN:",
    "LST:",
    "RSX:",
    "ERR:"
};

#define NDNAMES 6

extern int  atoi(char *);
static void fc_parse(struct fcb *, char *);

static char upcase(char c)
{
    if (islower(c))
        return toupper(c);
    return c;
}

struct fcb *getfcb()
{
    register struct fcb *fp;

    for (fp = _fcb ; fp < &_fcb[MAXFILE] ; fp++)
        if (!fp->use)
        {
            fp->use = U_READ;
            fp->rwp = 0;
            return fp;
        }
    return (struct fcb *)0;
}

void putfcb(struct fcb *fc)
{
    fc->use = 0;
}

uchar setfcb(struct fcb *fc, char *name)
{
    uchar           i,j;

    while (isspace(*name))
        ++name;
    /* Check device names */
    for (i = 0 ; i < NDNAMES ; ++i)
        for (j = 0; ;)
            if (dnames[i][j] != upcase(name[j]))
                break;
            else if (++j == DNAMLEN)
            {
                fc->use = i+U_CON;
                return 1;
            }
    /* Not a device; could be a file. */
    fc_parse(fc, name);
    return 0;
}

static void fc_parse(struct fcb *fc, char *name)
{
    char
        *cp,
        *np,
         c,
         num[3];

    fc->dr = 0;
    fc->uid = getuid();

    cp = name;
    if (cp[1]==':'  || cp[2]==':' || cp[3]==':')  /* Drive/user */
    {
        np = num;
        while (*cp != ':')
        {
            if (isdigit(*cp)) 
                *np++ = *cp;
            else if (fc->dr == 0)
                fc->dr = upcase(*cp) - 'A' + 1;  
            ++cp;
        }
        *np = 0;
        if (num[0])
            fc->uid=atoi(num);
        ++cp;
    }
    if (cp[1]==':' && fc->dr == 0 && !isdigit(cp[0]))  /* u:d: */
    {
        fc->dr=upcase(*cp) - 'A' + 1;
        cp += 2;
    }
    name = cp;
    cp = fc->name;
    while (*name != '.' && *name != '*' && *name > ' ' && cp < &fc->name[8])
        *cp++ = upcase(*name++);
    if (*name == '*')
        c = '?';
    else
        c = ' ';
    while (cp < &fc->name[8])
        *cp++ = c;
    while (*name && *name++ != '.')
        continue;
    while (*name > ' ' && *name != '*' && cp < &fc->ft[3])
        *cp++ = upcase(*name++);
    if (*name == '*')
        c = '?';
    else
        c = ' ';
    while (cp < &fc->ft[3])
        *cp++ = c;
    fc->ex = fc->nr = 0;
}
