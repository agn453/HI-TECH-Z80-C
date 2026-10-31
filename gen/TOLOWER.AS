	global	_tolower

	psect	text
_tolower:
	pop	de		;return address
	pop	hl
	push	hl
	push	de
	ld	a,h		;check for a char
	or	a
	ret	nz
	ld	a,l
	cp	'A'
	ret	c		;Less than a
	cp	'Z'+1
	ret	nc		;More than z
	sub	'A'-'a'
	ld	l,a
	ret
