extern int	unlink();

remove(s)
char *	s;
{
	return unlink(s);
}
