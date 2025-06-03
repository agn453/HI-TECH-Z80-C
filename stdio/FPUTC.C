/*
 *	fputc for HI-TECH C stdio
 */

#include	<stdio.h>

extern int _flsbuf(char, FILE *);

fputc(c, f)
register FILE *	f;
int	c;
{
	if(!(f->_flag & _IOWRT) || !(f->_flag & _IODIRN) && f->_cnt)
		return EOF;
	f->_flag |= _IODIRN;
	c &= 0xFF;
	if(f->_cnt == 0)
		f->_ptr = f->_base;
	if(f->_flag & _IOSEEKED && f->_flag & _IOAPPEND)
		fseek(f, 0L, SEEK_END);		/* force write at EOF */
	f->_flag &= ~_IOSEEKED;
	if(c == '\n' && !(f->_flag & _IOBINARY))
		fputc('\r', f);
	if(f->_cnt != f->_size) {
		f->_cnt++;
		*f->_ptr++ = c;
	} else
		return _flsbuf(c, f);
	if(c == '\n' && f->_flag & _IOLBF && fflush(f) == EOF)
		c = EOF;
	return c;
}
