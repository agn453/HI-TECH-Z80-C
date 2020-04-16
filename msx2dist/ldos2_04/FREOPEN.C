/*
 *	freopen.c - stdio freopen 
 */

#include	<stdio.h>

extern int	open(char *, int), creat(char *, int);

FILE *
freopen(name, mode, iob)
char *	name, * mode;
register FILE *	iob;
{
	uchar		c;

	fclose(iob);
	c = 0;
	iob->_flag &= _IONBF;
	switch(*mode) {

	case 'w':
		c++;
	case 'a':
		c++;
	case 'r':
		if(mode[1] == 'b')
			iob->_flag = _IOBINARY;
		break;

	}
	switch(c) {

	case 0:
		iob->_file = open(name, 0);
		break;

	case 1:
		if((iob->_file = open(name, 1)) >= 0)
			break;
		/* else fall through */
	case 2:
		iob->_file = creat(name, 0666);
		break;
	}
	if(iob->_file < 0)
		return (FILE *)NULL;
	if(!(iob->_flag & (_IONBF|_IOMYBUF)))
		iob->_base = _bufallo();
	if(iob->_base == (char *)-1) {
		iob->_base = (char *)0;
		close(iob->_file);
		iob->_flag = 0;
		return (FILE *)NULL;
	}
	iob->_ptr = iob->_base;
	iob->_cnt = 0;
	if(c)
		iob->_flag |= _IOWRT;
	else
		iob->_flag |= _IOREAD;
	if(iob->_base && c)
		iob->_cnt = BUFSIZ;
	if(c == 1)
		fseek(iob, 0L, 2);
	return iob;
}
