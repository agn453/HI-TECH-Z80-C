/*
 *	fclose - for HI-TECH C Z80 CP/M stdio
 */

#include	<stdio.h>
#include	<unixio.h>

fclose(f)
register FILE *	f;
{
	if(!(f->_flag & (_IOREAD|_IOWRT)))
		return(EOF);
	fflush(f);
	f->_flag &= ~(_IOREAD|_IOWRT|_IONBF|_IOLBF);
	if(f->_base && !(f->_flag & _IOMYBUF)) {
		_buffree(f->_base);
		f->_base = (char *)NULL;
	}
	if(close(fileno(f)) == -1 || f->_flag & _IOERR)
		return EOF;
	else
		return 0;
}
