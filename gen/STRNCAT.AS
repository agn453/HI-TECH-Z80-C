	psect	text
	global	_strncat, rcsv, cret

_strncat:
	call	rcsv
	push	hl

	jr	3f
4:
	inc	hl
3:
	ld	a,(hl)
	or	a
	jr	nz,4b

1:
	ld	a,c
	or	b
	jr	z,3f
	dec	bc
	ld	a,(de)
	ld	(hl),a
	inc	hl
	or	a
	jr	z,2f
	inc	de
	jr	1b
3:
	ld	(hl),0

2:	pop	hl
	jp	cret
