/*
 *	fgetc
 */

#include	<stdio.h>

extern int	_filbuf(FILE *);

fgetc(f)
register FILE *	f;
{
	int	c;

	if(f->_flag & _IOEOF || !(f->_flag & _IOREAD) || f->_flag & _IODIRN && f->_cnt) {
reteof:
		f->_flag |= _IOEOF;
		return EOF;
	}
	f->_flag &= ~_IODIRN;
	for(;;) {
		if(f->_cnt > 0) {
			f->_cnt--;
			c = (unsigned)*f->_ptr++;
		} else if(f->_flag & _IOSTRG)
			goto reteof;
		else
			c = _filbuf(f);
		if(!(f->_flag & _IOBINARY)) {
			if(c == 032) {
				if(f->_base) {
					f->_cnt++;
					f->_ptr--;
				}
				goto reteof;
			} else if(c == '\r')
				continue;
		}
		return c;
	}
}
