	psect	text
	global	alxor, llxor

alxor:
llxor:
	exx
	pop	hl
	exx
	pop	bc
	ld	a,c
	xor	e
	ld	e,a
	ld	a,b
	xor	d
	ld	d,a
	pop	bc
	ld	a,c
	xor	l
	ld	l,a
	ld	a,b
	xor	h
	ld	h,a
	exx
	push	hl
	exx
	ret
