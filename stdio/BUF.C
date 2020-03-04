#include	<stdio.h>

extern char *sbrk();

static union stdbuf
{
	char		bufarea[BUFSIZ];
	union stdbuf   *link;
} *freep;

char *_bufallo()
{
    register union stdbuf *pp;

    if (pp = freep)
	freep = pp->link;
    else
	pp = (union stdbuf *)sbrk(BUFSIZ);
    return pp->bufarea;
}

void _buffree(char *pp)
{
    register union stdbuf *up;

    up = (union stdbuf *)pp;
    up->link = freep;
    freep = up;
}
