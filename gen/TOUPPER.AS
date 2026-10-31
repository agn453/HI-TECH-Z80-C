	global	_toupper

	psect	text
_toupper:
	pop	de		;return address
	pop	hl
	push	hl
	push	de
	ld	a,h		;check for a char
	or	a
	ret	nz
	ld	a,l
	cp	'a'
	ret	c		;Less than a
	cp	'z'+1
	ret	nc		;More than z
	sub	'a'-'A'
	ld	l,a
	ret
