	psect	text
	global	aland, lland

aland:
lland:
	exx
	pop	hl
	exx
	pop	bc
	ld	a,c
	and	e
	ld	e,a
	ld	a,b
	and	d
	ld	d,a
	pop	bc
	ld	a,c
	and	l
	ld	l,a
	ld	a,b
	and	h
	ld	h,a
	exx
	push	hl
	exx
	ret
