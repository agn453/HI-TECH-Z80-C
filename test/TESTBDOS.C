/*----------------------------------------------------------------------*\
 | testbdos								|
 |									|
 | A small program to test the updated bdos() function of Hi-Tech C	|
 |									|
 | Jon Saxton	April 2014						|
\*----------------------------------------------------------------------*/

#include <cpm.h>
#include <stdio.h>

void test(short func, void *data)
{
    int e;
    short rc = bdose(func, data);
    e = errno;
    printf("rc = %04x, errno = %d\n", rc, errno);
    errno = e;
    if (errno)
	perror("test");
}

void vogon(char *fn)	/* Just for the debugger */
{
    char fcb[50];
    struct _pfcb
    {
        char *str;
        char *fcb;
    } pfcb;
    pfcb.fcb = fcb;
    pfcb.str = fn;
    test(152, &pfcb);
    test(15, fcb);
}

int main(int argc, char *argv[])
{
    int e;
    char s[6];
    printf("First some perror() tests ...\n\n");
    for (e=0; e<28; ++e)
    {
        sprintf(s,"%2d",e);
        errno = e;
        perror(s);
    }
    putchar('\n');
    bdose(0x2D, 0xFF);
/*
    test(14,(void *)3);
    test(14,(void *)6);
*/
/*
    vogon("b:zpm3ldr.rel;asdf");
    if (argc > 1)
       vogon(argv[1]);
*/
    return 0;
}
