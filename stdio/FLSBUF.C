/*
 *	_flsbuf for HI-TECH C stdio
 */

#include	<stdio.h>
#include	<unixio.h>

_flsbuf(char c, register FILE * f)
{
	if(!(f->_flag & _IOWRT)) {
		f->_flag |= _IOERR|_IOEOF;
		f->_cnt = 0;
		return EOF;
	}
	if(f->_size == 0) {
		if(f->_flag & (_IONBF|_IOMYBUF) || isatty(fileno(f)) ||
			(f->_base = _bufallo()) == (char *)-1) {
			f->_base = (char *)0;
			f->_size = 0;
			f->_flag |= _IONBF;
			f->_cnt = 0;
			if(write(fileno(f), &c, 1) == 1)
				return c & 0xFF;
			f->_flag |= _IOERR|_IOEOF;
			return EOF;
		} else
			f->_size = BUFSIZ;
	} else if(write(fileno(f), f->_base, f->_cnt) != f->_cnt)
		f->_flag |= _IOERR|_IOEOF;
	f->_cnt = 1;
	*f->_base = c;
	f->_ptr = f->_base+1;
	if(f->_flag & _IOERR)
		return(EOF);
	return c & 0xFF;
}
