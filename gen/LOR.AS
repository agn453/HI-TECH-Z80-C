	psect	text
	global	alor, llor

alor:
llor:
	exx
	pop	hl
	exx
	pop	bc
	ld	a,c
	or	e
	ld	e,a
	ld	a,b
	or	d
	ld	d,a
	pop	bc
	ld	a,c
	or	l
	ld	l,a
	ld	a,b
	or	h
	ld	h,a
	exx
	push	hl
	exx
	ret
