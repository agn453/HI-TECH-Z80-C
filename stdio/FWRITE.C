#include	<stdio.h>
#include	<string.h>

/*
 *	fwrite
 */

fwrite(buf, size, nitems, stream)
void *	buf;
register FILE *	stream;
unsigned	size, nitems;
{
	register unsigned	count, n;
	register char *		ptr;

	if(!(stream->_flag & _IOWRT) || !(stream->_flag & _IODIRN) && stream->_cnt)
		return 0;
	stream->_flag |= _IODIRN;
	if(stream->_cnt == 0)
		stream->_ptr = stream->_base;
	if(stream->_flag & _IOSEEKED && stream->_flag & _IOAPPEND)
		fseek(stream, 0L, SEEK_END);	/* force write at EOF */
	stream->_flag &= ~_IOSEEKED;
	count = size * nitems;
	ptr = buf;
	while(count) {
		if(stream->_base && stream->_flag & _IOBINARY) {
			n = stream->_size - stream->_cnt;
			if(n > count)
				n = count;
			if(n) {
				memcpy(stream->_ptr, ptr, n);
				ptr += n;
				count -= n;
				stream->_ptr += n;
				stream->_cnt += n;
				if(stream->_cnt == stream->_size)
					fflush(stream);
				continue;
			}
		}
		if(putc(*ptr++, stream) == EOF)
			break;
		else
			--count;
	}
	return(nitems - (count+size-1)/size);
}
