/*
 *	Setbuf for HI-TECH C stdio
 */

#include	<stdio.h>

void setbuf(f, c)
register FILE *	f;
char *		c;
{
	if(c)
		setvbuf(f, c, _IOFBF, BUFSIZ);
	else
		setvbuf(f, c, _IONBF, 0);
}

setvbuf(register FILE * stream, register char * buf, int mode, size_t size)
{
	switch(mode) {
	case _IONBF:	/* Not buffered */
		if(!(stream->_flag & _IOMYBUF) && stream->_base)
			_buffree(stream->_base);
		stream->_base = stream->_ptr = 0;
		stream->_flag &= ~(_IOMYBUF|_IOLBF);
		stream->_flag |= _IONBF;
		break;

	case _IOLBF:	/* Line buffered or */
	case _IOFBF:	/* Fully buffered */
		if(buf)
			stream->_flag |= _IOMYBUF;
		if(!buf && !(buf = stream->_base)) {
			if(!(buf = _bufallo()))
				return -1;
			size = BUFSIZ;
		}
		stream->_base = buf;
		stream->_size = size;
		stream->_flag &= ~(_IONBF|_IOLBF);
		stream->_flag |= mode;
		break;

	default:
		return -1;
	}
	stream->_cnt = 0;
	return 0;
}
