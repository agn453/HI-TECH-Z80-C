;	abs(i) returns the absolute value of i

	global	_abs

	psect	text
_abs:
	pop	de		;Return address
	pop	hl
	push	hl
	push	de
	bit	7,h		;Negative?
	ret	z		;no, leave alone
	ex	de,hl
	ld	hl,0
	or	a		;Clear carry
	sbc	hl,de
	ret
