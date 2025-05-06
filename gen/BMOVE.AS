;	bmove(from, to, count)

	global	_bmove, _movmem
	psect	text

_movmem:
_bmove:
	pop	hl		;return address
	exx
	pop	hl		;from
	pop	de		;to
	pop	bc		;count
	ld	a,b
	or	c
	jr	z,1f
	ldir
1:
	push	bc		;stack is as it was
	push	de
	push	hl
	exx
	jp	(hl)
