	global	_xtoi, _ishex
	psect	text
hexdig:	cp	'0'
	ret	c
	cp	'9'+1
	jr	nc,1f
	sub	'0'
	ret

1:	cp	'A'
	ret	c
	cp	'F'+1
	jr	nc,2f
	sub	'A'-0Ah
	ret

2:	cp	'a'
	ret	c
	cp	'f'+1
	ccf
	ret	c
	sub	'a'-0ah
	ret

_xtoi:	pop	bc	;return address
	pop	de
	push	de
	push	bc
	ld	hl,0
1:
	ld	a,(de)
	inc	de
	call	hexdig
	ret	c
	add	hl,hl
	add	hl,hl
	add	hl,hl
	add	hl,hl
	ld	c,a
	ld	b,0
	add	hl,bc
	jr	1b

_ishex:	pop	hl
	pop	de
	push	de
	push	hl
	ld	a,e
	ld	hl,0
	call	hexdig
	ret	c
	inc	hl
	ret
