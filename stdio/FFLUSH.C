/*
 *  fflush for HI-TECH C Z80 CP/M stdio
 */

#include	<stdio.h>
#include	<unixio.h>

int fflush(register FILE *f)
{
	if (f->_flag & _IOREAD && !(f->_flag & _IODIRN)) {
		/* File in read mode and not modified */
		f->_cnt = 0;
		f->_ptr = f->_base;
		if(f->_flag & _IOERR)
			return EOF;
		else
			return 0;
		}
	if(!(f->_flag & _IOWRT) || !(f->_flag & _IODIRN) ||
		f->_base == NULL || f->_cnt == 0)
			return 0;
	if(write(fileno(f), f->_base, f->_cnt) != f->_cnt)
		f->_flag |= _IOERR|_IOEOF;
	f->_cnt = 0;
	f->_ptr = f->_base;
	if(f->_flag & _IOERR)
		return EOF;
	else
		return 0;
}
