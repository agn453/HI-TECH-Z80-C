/*----------------------------------------------------------------------*\
 | testwild.c								|
 |									|
 | This program is designed to test wildcard expansion using the stdio	|
 | routine _getargs().							|
 |									|
 | Invoke with one or more file names (which may contain wildcards).	|
 | The program will display the names of any files which match.		|
\*----------------------------------------------------------------------*/

#include <stdio.h>
#include <cpm.h>
#include <sys.h>

int main(int argc, char *argv[])
{
    int a;
    extern int _argc_;
    char **args;

    args = _getargs((char *)0x81, "TESTWILD");

    printf("Program name: %s\n", args[0]);

    for (a = 1; a < _argc_; ++a)
    {
        printf("%2d: %s\n", a, args[a]);
    }
}
