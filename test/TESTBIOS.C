/*----------------------------------------------------------------------*\
 | TESTBIOS								|
 |									|
 | A small program to test BIOS calls with a variety of parameters.	|
 |									|
 | Written for CP/M 3.  May not work on CP/M 2.2.			|
\*----------------------------------------------------------------------*/

#include <cpm.h>
#include <stdio.h>

char
    src[] = "Did this work?",
    dst[sizeof src];

int main(int argc, char *argv[], char *z3env)
{
    short
	rc;

    if (z3env)
	printf("Z3ENV at %04x\n");
    else
	puts("Z3ENV is not established.  (This is not an error.)");

    for (rc = 1; rc < argc; ++rc)
	printf("%2d: %s\n", argv[rc]);

    /* BIOS call with no parameters */
    printf("Press a key: ");
    rc = bios(3,0xBCBC,0xDEDE);
    printf("rc = %04x, character = ", rc);

    /* BIOS call with one parameter (in BC) */
    rc = bios(4, rc, 0xDEDE);

    /* BIOS call with one parameter (in A) */
    rc = bios(27,1,0xBCBC,0xDEDE,0x1414);

    puts("\nThe next message should say \"Did this work?\"");

    /* BIOS call with three parameters (BC, DE & HL) */
    bios(29,0x0101);
    bios(25,sizeof src, src, dst);
    puts(dst);
}
