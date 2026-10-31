	psect	text
	global	_strcmp

_strcmp:	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc

1:	ld	a,(de)
	cp	(hl)
	jr	nz,2f
	inc	de
	inc	hl
	or	a
	jr	nz,1b
	ld	hl,0
	ret

2:	ld	hl,1
	ret	nc
	dec	hl
	dec	hl
	ret
