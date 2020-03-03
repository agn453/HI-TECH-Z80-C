memset(p, n, c)
register char *	p;
register int	n;
char		c;
{
	while(n--)
		*p++ = c;
}
