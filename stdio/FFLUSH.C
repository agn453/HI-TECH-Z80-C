/*
 *  fflush for Zios stdio
 */

#include    <stdio.h>

extern int  write(int, void *, int);

int fflush(register FILE *f)
{
    unsigned    cnt;

    if (!(f->_flag & _IOWRT)			/* File not writeable */
        || !(f->_flag & (_IODIRN | _IOWROTE))	/* File in read mode */
        || f->_base == (char *)NULL		/* Not buffered */
        || (cnt = BUFSIZ - f->_cnt) == 0)	/* Nothing to write */
        return 0;

    if (write(fileno(f), f->_base, cnt) != cnt)
        f->_flag |= _IOERR;
    f->_flag &= (0xFFFF-_IOWROTE);
    f->_cnt = BUFSIZ;
    f->_ptr = f->_base;
    if (f->_flag & _IOERR)
        return(EOF);
    return 0;
}
