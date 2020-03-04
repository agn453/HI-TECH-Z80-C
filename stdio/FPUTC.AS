;	fputc for stdio - this version in assembler for speed

;	/*
;	 *	fputc for Zios stdio
;	 */
;	
;	#include	<stdio.h>
;	
;	fputc(c, f)
;	register FILE *	f;
;	uchar	c;
;	{
;		if(!(f->_flag & _IOWRT))
;			return EOF;
;		if(!(f->_flag & _IODIRN))
;			{
;			f->_cnt = BUFSIZ;
;			f->_flag |= _IODIRN;
;			}
;               f->_flag |= _IOWROTE;
;		if((f->_flag & _IOBINARY) == 0 && c == '\n')
;			fputc('\r', f);
;		if(f->_cnt > 0) {
;			f->_cnt--;
;			*f->_ptr++ = c;
;		} else
;			return _flsbuf(c, f);
;		return c;
;	}

*Include	stdio.i

	global	_fputc, __flsbuf
	psect	text
_fputc:
	pop	de			;return address
	pop	bc			;character argument
	ld	b,0			;so zero the top byte
	ex	(sp),iy			;save iy and get file pointer
	bit	_IOWRT_BIT,(iy+flag)	;are we reading
	jr	z,reteof
	bit	1,(iy+flag+1)		;_IODIRN_BIT
	jr	nz,4f	
	set	1,(iy+flag+1)		;Set _IODIRN
	ld	(iy+cnt),0
	ld	(iy+cnt+1),2		;512
4:	set	2,(iy+flag+1)		;_IOWROTE_BIT
	bit	_IOBINARY_BIT,(iy+flag)	;binary mode?
	jr	nz,2f			;yes, just return
	ld	a,c			;is it a newline?
	cp	NEWLINE
	jr	nz,2f			;no
	push	bc			;save thingos
	push	de
	push	iy			;file argument
	ld	hl,RETURN
	push	hl
	call	_fputc
	pop	hl			;unjunk stack
	pop	bc
	pop	de
	pop	bc
2:
	ld	l,(iy+cnt)
	ld	h,(iy+cnt+1)
	ld	a,l			;check count
	or	h
	jr	z,1f			;no room at the inn
	bit	_IONBF_BIT,(iy+flag)	;Not buffered?
	jr	nz,1f			;Then spit it straight out
	dec	hl			;update count
	ld	(iy+cnt),l
	ld	(iy+cnt+1),h
	ld	l,(iy+ptr)
	ld	h,(iy+ptr+1)		;get pointer
	ld	(hl),c			;store character
	inc	hl			;bump pointer
	ld	(iy+ptr),l
	ld	(iy+ptr+1),h
3:
	ex	(sp),iy			;restore iy
	push	bc			;fix stack up
	push	de
	ld	l,c
	ld	h,b			;return the character
	ret

1:
	ex	(sp),iy			;restore the stack to what it was
	push	bc
	push	de			;return address and all
	jp	__flsbuf		;let flsbuf handle it

reteof:
	ld	bc,-1
	jr	3b
