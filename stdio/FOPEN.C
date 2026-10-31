/*
 *	fopen.c - stdio fopen 
 */

#include	<stdio.h>


FILE *
fopen(name, mode)
char *	name, * mode;
{
	register FILE *	f;

	for(f = _iob ; f != &_iob[_NFILE] ; f++)
		if(!(f->_flag & (_IOREAD|_IOWRT)))
			break;
	if(f == &_iob[_NFILE])
		return((FILE *)NULL);
	return freopen(name, mode, f);
}
