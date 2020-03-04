;	/*
;	 *	fgetc for Zios stdio
;	 */
;	
;	#include	<stdio.h>
;	
;	#define	CPMEOF	032		/* ctrl-Z */
;	
;	fgetc(f)
;	register FILE *	f;
;	{
;		int	c;
;	
;		if(f->_flag & _IOEOF || !(f->_flag & _IOREAD)) {
;	reteof:
;			f->_flag |= _IOEOF;
;			return EOF;
;		}
;	loop:
;		if(f->_flag & _IODIRN) 
;		{
;			c=_filbuf(f);
;			f->_flag &= !_IODIRN;
;		}
;		else if(f->_cnt > 0) {
;			c = (unsigned)*f->_ptr++;
;			f->_cnt--;
;		} else if(f->_flag & _IOSTRG)
;			goto reteof;
;		else
;			c = _filbuf(f);
;		if(f->_flag & _IOBINARY)
;			return c;
;		if(c == '\r')
;			goto loop;
;		if(c == CPMEOF) {
;			f->_cnt++;
;			f->_ptr--;
;			goto reteof;
;		}
;		return c;
;	}

;	The assembler version of the above routine

*Include	stdio.i
	global	_fgetc, __filbuf
	psect	text

_fgetc:
	pop	de			;get return address off stack
	ex	(sp),iy			;save iy and get arguement into iy
	ld	a,(iy+flag)		;get flag bits
	bit	_IOREAD_BIT,a
	jr	z,reteof		;return EOF if not open for read
	bit	_IOEOF_BIT,a		;Already seen EOF?
	jr	nz,reteof		;yes, repeat ourselves

	bit	1,(iy+flag+1)		;_IODIRN_BIT
	jr	z,loop
	res	1,(iy+flag+1)
	push	iy
	call	__filbuf
	pop	bc
	ld	a,l
	bit	7,h			;EOF
	jr	nz,reteof
	jr	2f			;Got a byte

loop:
	ld	l,(iy+cnt)
	ld	h,(iy+cnt+1)
	ld	a,l
	or	h			;any bytes left?
	jr	z,1f			;no, go get some more
	dec	hl
	ld	(iy+cnt),l		;update count
	ld	(iy+cnt+1),h
	ld	l,(iy+ptr)		;get the pointer
	ld	h,(iy+ptr+1)
	ld	a,(hl)
	inc	hl
	ld	(iy+ptr),l		;update pointer
	ld	(iy+ptr+1),h
2:
	bit	_IOBINARY_BIT,(iy+flag)	;Binary mode?
	jr	z,3f			;no, check for EOF etc
retch:
	ld	l,a			;return the character in a
	ld	h,0
	ex	(sp),iy			;restore iy
	push	de			;put return address back
	ret				;with char in hl

3:
	cp	RETURN			;carriage return
	jr	z,loop			;yes, get another instead
	cp	CPMEOF			;end of file?
	jr	nz,retch		;no, return it!
	ld	a,(iy+base)		;buffered?
	or	(iy+base+1)
	jr	z,reteof		;yup, leave count alone
	ld	l,(iy+cnt)
	ld	h,(iy+cnt+1)
	inc	hl			;reset count
	ld	(iy+cnt),l
	ld	(iy+cnt+1),h
	ld	l,(iy+ptr)
	ld	h,(iy+ptr+1)
	dec	hl			;reset pointer
	ld	(iy+ptr),l
	ld	(iy+ptr+1),h
reteof:
	set	_IOEOF_BIT,(iy+flag)	;note EOF
	ld	hl,EOF
	ex	(sp),iy			;restore iy
	push	de
	ret				;return with EOF in hl

1:
	bit	_IOSTRG_BIT,(iy+flag)	;end of string?
	jr	nz,reteof		;yes, return EOF
	push	de			;save de
	push	iy			;pass iy as argument
	call	__filbuf		;refill the buffer
	ld	a,l			;the returned value
	pop	bc
	pop	de			;return address in de again
	bit	7,h
	jr	nz,reteof		;returned EOF
	jr	2b
