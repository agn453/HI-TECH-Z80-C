/*
 *	_filbuf for HI-TECH C stdio
 */

#include	<stdio.h>
#include	<unixio.h>

_filbuf(f)
register FILE *	f;
{
	register FILE *	op;

	f->_cnt = 0;
	if(!(f->_flag & _IOREAD))
		return(EOF);
	for(op = _iob ; op != _iob + _NFILE ; op++)
		if((op->_flag & (_IOWRT|_IOLBF)) == (_IOWRT|_IOLBF))
			fflush(op);
	if(f->_size == 0) {
		uchar	c;
		f->_cnt = 0;
		if(read(fileno(f), &c, 1) == 1)
			return(c);
		f->_flag |= _IOEOF;
		return(EOF);
	}
	if((f->_cnt = read(fileno(f), f->_base, f->_size)) <= 0) {
		if(f->_cnt == 0)
			f->_flag |= _IOEOF;
		else
			f->_flag |= _IOERR|_IOEOF;
		f->_ptr = f->_base;
		f->_cnt = 0;
		return(EOF);
	}
	f->_ptr = f->_base;
	f->_cnt--;
	return((unsigned)*f->_ptr++);
}
