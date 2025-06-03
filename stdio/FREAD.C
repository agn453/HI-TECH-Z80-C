#include	<stdio.h>
#include	<string.h>

/*
 *	fread for HI-TECH C stdio
 */

fread(buf, size, nitems, stream)
void *	buf;
register FILE *	stream;
unsigned	size, nitems;
{
	register char *		ptr;
	register unsigned	count, n;
	short			c;

	if(stream->_flag & _IOEOF || !(stream->_flag & _IOREAD) || stream->_flag & _IODIRN && stream->_cnt) {
		stream->_flag |= _IOEOF;
		return 0;
	}
	stream->_flag &= ~_IODIRN;
	count = size * nitems;
	ptr = buf;
	while(count && !feof(stream)) {
		if(stream->_cnt && stream->_size && stream->_flag & _IOBINARY) {
			n = stream->_cnt;
			if(n > count)
				n = count;
			if(n) {
				memcpy(ptr, stream->_ptr, n);
				ptr += n;
				count -= n;
				stream->_ptr += n;
				stream->_cnt -= n;
				continue;
			}
		}
		if((c = getc(stream)) == EOF)
			break;
		else {
			--count;
			*ptr++ = c;
		}
	}
	return(nitems - (count+size-1)/size);
}
