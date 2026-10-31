	psect	text
digit:	sub	'0'
	ret	c
	cp	10
	ccf
	ret

	global	_atoi
_atoi:	pop	bc	;return address
	pop	de
	push	de
	push	bc
	ld	hl,0
1:
	ld	a,(de)
	inc	de
	cp	' '
	jr	z,1b
	cp	'	'	;tab
	jr	z,1b
	dec	de		;point to 1st non blank char
	cp	'-'
	jr	z,3f
	cp	'+'
	jr	nz,2f
	or	a		;reset zero flag
3:
	inc	de
2:	ex	af,af'
1:
	ld	a,(de)
	inc	de
	call	digit
	jr	c,3f
	add	hl,hl
	ld	c,l
	ld	b,h
	add	hl,hl
	add	hl,hl
	add	hl,bc
	ld	c,a
	ld	b,0
	add	hl,bc
	jr	1b

3:
	ex	af,af'
	ret	nz
	ex	de,hl
	ld	hl,0
	sbc	hl,de
	ret
