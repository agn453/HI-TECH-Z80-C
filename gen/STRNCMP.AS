	psect	text
	global	_strncmp, rcsv, cret


_strncmp:
	call	rcsv

1:	ld	a,c
	or	b
	jp	z,3f
	dec	bc
	ld	a,(de)
	cp	(hl)
	jr	nz,2f
	inc	de
	inc	hl
	or	a
	jr	nz,1b
3:
	ld	hl,0
	jp	cret

2:	ld	hl,1
	jp	c,cret
	dec	hl
	dec	hl
	jp	cret
