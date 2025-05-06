	psect	text
	global	rcsv, cret, _rindex

_rindex:
	call	rcsv

	ld	bc,0
	jr	5f
6:
	inc	hl
	inc	bc
5:
	ld	a,(hl)
	or	a
	jr	nz,6b
1:
	dec	hl
	ld	a,c
	or	b
	jr	z,2f
	dec	bc
	ld	a,(hl)
	cp	e
	jr	nz,1b
4:	jp	cret

2:	ld	hl,0
	jp	4b
