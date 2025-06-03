#include	<stdio.h>

/*
 *	ungetc for HI-TECH C
 */

ungetc(c, stream)
int		c;
register FILE *	stream;
{
	if(c == EOF || !(stream->_flag & _IOREAD) || stream->_flag & _IODIRN ||
		stream->_base == (char *)NULL || stream->_cnt == stream->_size)
		return(EOF);
	if(stream->_ptr == stream->_base)
		stream->_ptr++;
	else
		stream->_cnt++;
	*--stream->_ptr = c;
	return(c);
}
