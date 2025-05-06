;	ftol - convert floating to long, by using lower bits can also
;	be used to convert from float to int or char

	psect	text
	global	ftol
	global	alrsh, allsh, negmant

ftol:
	bit	7,h		;test sign
	call	nz,negmant	;negate mantissa if required
	ld	a,h		;get exponent
	res	7,a		;mask sign off
	sub	64+24		;remove offset
	ld	b,a		;save shift count
	ld	a,h		;get exponent, sign
	rla
	sbc	a,a		;sign extend
	ld	h,a		;put back
	bit	7,b		;test sign
	jp	z,allsh		;shift it left
	ld	a,b		;get the count
	neg			;make +ve
	dec	a		;and reduce it one
	ld	b,a		;put back in b
	call	nz,alrsh	;shift right
	ex	de,hl
	ld	bc,1		;add one for rounding
	add	hl,bc
	ld	b,c
	ex	de,hl
	jp	nc,alrsh	;and shift down one more
	inc	hl		;add in carry first
	jp	alrsh
