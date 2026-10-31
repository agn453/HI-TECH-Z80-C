	psect	text
	global	_strlen
_strlen:	pop	hl
	pop	de
	push	de
	push	hl
	ld	hl,0

1:	ld	a,(de)
	or	a
	ret	z
	inc	hl
	inc	de
	jr	1b
