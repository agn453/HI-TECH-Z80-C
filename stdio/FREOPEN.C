/*
 *	freopen.c - HI-TECH C stdio freopen 
 */

#include	<stdio.h>
#include	<unixio.h>


FILE *
freopen(name, mode, iob)
char *	name, * mode;
register FILE *	iob;
{
	uchar		c, rw;

	fclose(iob);
	c = rw = 0;
	iob->_flag &= _IOMYBUF;
	switch(*mode) {

	case 'w':
		c++;
	case 'a':
		c++;
	case 'r':
		break;
	default:
		return NULL;
	}
	rw = 0;
	while(*++mode)
		if(*mode == 'b')
			iob->_flag |= _IOBINARY;
		else if(*mode == '+') {
			iob->_flag |= _IORW;
			rw = 2;
		} else 
			break;
	
	switch(c) {

	case 0:
		iob->_file = open(name, rw);
		break;

	case 1:
		if((iob->_file = open(name, rw ? 2 : 1)) >= 0)
			break;
		/* else fall through */
	case 2:
		iob->_file = creat(name, 0666);
		if(iob->_file >= 0 && rw) {
			close(iob->_file);
			iob->_file = open(name, rw);
		}
		break;
	}
	if(iob->_file < 0)
		return (FILE *)NULL;
	if(!(iob->_flag & (_IONBF|_IOMYBUF))) {
		iob->_base = _bufallo();
		iob->_size = BUFSIZ;
	}
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
	if(c == 1)
		fseek(iob, 0L, 2);
	return iob;
}
