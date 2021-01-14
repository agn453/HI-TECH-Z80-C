/*----------------------------------------------------------------------*\
 | testargs.c								|
 |									|
 | This program is designed to test wildcard expansion without calling	|
 | routine _getargs().							|
 |									|
 | Invoke with one or more file names (which may contain wildcards).	|
 | The program will display the names of any files which match.		|
 |									|
 | Note that there is no easy way to set the program name (argv[0]).	|
\*----------------------------------------------------------------------*/

#include <stdio.h>
#include <cpm.h>
#include <sys.h>

void view(char *);

int main(int argc, char *argv[])
{
    int a;
    bdos(45,255);

    for (a = 1; a < argc; ++a)
    {
        printf("%2d: %s\n", a, argv[a]);
        view(argv[a]);
    }
}

void view(char *fn)
{
    FILE  *f;
    char  *bp, buf[200];

    f = fopen(fn, "r");
    while (bp=fgets(buf, 200, f))
        printf("%s", bp);
    fclose(f);
}
