memcpy(d, s, n)
register char *	d, * s;
register int	n;
{
	while(n--)
		*d++ = *s++;
}
