	psect	text
	global	_strcpy

_strcpy:	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc
	ld	c,e
	ld	b,d		;save destination pointer

1:	ld	a,(hl)
	ld	(de),a
	inc	de
	inc	hl
	or	a
	jr	nz,1b
	ld	l,c
	ld	h,b
	ret
