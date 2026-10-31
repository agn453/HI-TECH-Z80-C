	global	_max
	psect	text

_max:
	pop	bc		;return address
	pop	de		;arg 1
	pop	hl		;arg 2
	push	hl
	push	de
	push	bc
	push	hl		;save it
	or	a		;clear carry
	sbc	hl,de		;compare
	pop	hl		;restore
	ret	nc		;return if greater or equal
	ex	de,hl		;otherwise returnt the other
	ret
