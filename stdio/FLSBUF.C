/*
 *	_flsbuf for Zios stdio
 */

#include	<stdio.h>

extern int	write(int, void *, int);

_flsbuf(c, f)
register FILE *	f;
uchar	c;
{
	if(f->_flag & _IOWRT) {
		if(f->_base == (char *)NULL) {
			f->_cnt = 0;
			if(write(fileno(f), &c, 1) == 1)
				return(c);
			f->_flag |= _IOERR;
			return(EOF);
		}
		if(write(fileno(f), f->_base, BUFSIZ) != BUFSIZ)
			f->_flag |= _IOERR;
		f->_cnt = BUFSIZ-1;
		*f->_base = c;
		f->_ptr = f->_base+1;
	} else {
		f->_flag |= _IOERR;
		f->_cnt = 0;
	}
	if(f->_flag & _IOERR)
		return(EOF);
	return(c);
}
