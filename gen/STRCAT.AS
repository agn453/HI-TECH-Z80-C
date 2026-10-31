	psect	text
	global	_strcat

_strcat:
	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc
	ld	c,e		;save destination pointer
	ld	b,d

1:	ld	a,(de)
	or	a
	jr	z,2f
	inc	de
	jr	1b

2:	ld	a,(hl)
	ld	(de),a
	or	a
	jr	z,3f
	inc	de
	inc	hl
	jr	2b

3:
	ld	l,c	;restore destination
	ld	h,b
	ret
