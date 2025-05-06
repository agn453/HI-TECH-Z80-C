blkcpy(dp, sp, n)
	register char *dp, *sp;
	register unsigned n;
{
	while (n--)
		*dp++ = *sp++;
}
