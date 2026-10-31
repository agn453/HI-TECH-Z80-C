;	These functions allow the mantissa and exponent of floating
;	numbers to be manipulate separately.


	psect	text
	global	_frexp, _ldexp

;	double frexp(value, eptr)
;	double value;
;	int *  eptr;

_frexp:
	push	ix
	ld	ix,0
	add	ix,sp
	ld	a,(ix+7)		;get old exponent
	ld	b,a			;save sign bit
	and	80h			;mask it out
	add	a,64			;add in bias
	ld	d,a			;store new exponent back
	ld	a,b			;now get exponent
	and	7Fh			;clea sign bit
	sub	64			;remove bias
	ld	l,(ix+8)		;get pointer
	ld	h,(ix+9)
	ld	(hl),a
	inc	hl
	rla
	sbc	a,a
	ld	(hl),a			;store upper byte
	ld	l,(ix+4)		;now get value to return
	ld	h,(ix+5)
	ld	e,(ix+6)
	ex	de,hl			;already have sign in d
	pop	ix
	ret

_ldexp:
	push	ix
	ld	ix,0
	add	ix,sp
	ld	a,(ix+8)
	and	7Fh
	ld	c,a
	ld	a,(ix+7)
	ld	h,a
	and	80h
	ld	b,a
	ld	a,h
	add	a,c
	and	7Fh
	or	b
	ld	h,a
	ld	l,(ix+6)
	ld	d,(ix+5)
	ld	e,(ix+4)
	pop	ix
	ret
