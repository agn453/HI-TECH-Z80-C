/*
 *	Setbuf for Zios stdio
 */

#include	<stdio.h>

setbuf(f, c)
register FILE *	f;
char *		c;
{
	if(!(f->_flag & _IOMYBUF) && f->_base)
		_buffree(f->_base);
	f->_cnt = 0;
	if(!(f->_base = f->_ptr = c)) {
		f->_flag &= ~_IOMYBUF;
		return;
	}
	f->_flag |= _IOMYBUF;
	if(f->_flag & _IOWRT)
		f->_cnt = BUFSIZ;
}
