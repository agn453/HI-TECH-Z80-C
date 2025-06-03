extern void _cleanup(void);
extern void _exit(int);

exit(v)
{
	_cleanup();
	_exit(v);
}
