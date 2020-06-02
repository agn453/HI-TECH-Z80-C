; strchr(char *s, int c)
; version that can find the closing \0 by Arnold M

	psect	text
	global	rcsv, cret, _strchr	; also equivalent to _index

_strchr:
	pop	bc
	pop	hl
	pop	de
	push	de
	push	hl
	push	bc

	jr	3f
1:
	inc	hl
3:
	ld	a,(hl)
	cp	e ; check for a match first, e may be zero
	ret	z

	or	a
	jr	nz,1b
	ld	hl,0
	ret
