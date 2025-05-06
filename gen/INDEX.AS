	psect	text
	global	rcsv, cret, _index

_index:	call	rcsv

	jr	3f
1:
	inc	hl
3:
	ld	a,(hl)
	or	a
	jr	z,2f
	cp	e
	jr	nz,1b
4:	jp	cret

2:	ld	hl,0
	jp	4b
