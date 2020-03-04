#include	<stdio.h>

extern	int	errno;
extern	char *	sys_err[];
extern	int	sys_ner;

static void ps(char *s)
{
    while(*s)
        putc(*s++, stderr);
}

void perror(char *s)
{
    ps(s);
    putc(':', stderr);
    if (errno < sys_ner)
        ps(sys_err[errno]);
    else
        ps("Unknown error");
    putc('\n', stderr);
}
