	global	_in, _out, _inp, _outp
	psect	text

_in:
_inp:
	pop	hl		;return address
	pop	bc		;port address
	push	bc
	push	hl
	in	l,(c)		;read port
	ld	h,0		;zero extend it
	ret

_out:
_outp:
	pop	hl		;return address
	pop	bc		;port address
	pop	de		;data
	push	de
	push	bc
	push	hl
	out	(c),e		;output the data
	ld	l,c		;return value in hl also
	ld	h,0
	ret
