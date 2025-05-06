/*
 *	_filbuf for Zios stdio
 */

extern int	read(int, void *, int);

#include	<stdio.h>
_filbuf(f)
register FILE *	f;
{
	f->_cnt = 0;
	if(!(f->_flag & _IOREAD))
		return(EOF);
	if(f->_base == (char *)NULL) {
		uchar	c;
		f->_cnt = 0;
		if(read(fileno(f), &c, 1) == 1)
			return(c);
		f->_flag |= _IOEOF;
		return(EOF);
	}
	if((f->_cnt = read(fileno(f), f->_base, BUFSIZ)) <= 0) {
		if(f->_cnt == 0)
			f->_flag |= _IOEOF;
		else
			f->_flag |= _IOERR;
		return(EOF);
	}
	f->_ptr = f->_base;
	f->_cnt--;
	return((unsigned)*f->_ptr++);
}
